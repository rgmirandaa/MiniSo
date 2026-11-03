#include "concurrencia.h"
#include <iostream>
#include <string>
#include <vector>
#include <new>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define MAX_BUFFER 256

static const useconds_t ESPERA_PRODUCTOR_US = 70000;   // 70 ms
static const useconds_t ESPERA_CONSUMIDOR_US = 110000; // 110 ms

static const int NUM_PRODUCTORES = 2;
static const int NUM_CONSUMIDORES = 2;

// Estructura para el buffer circular compartido
struct BufferCompartido {
    int datos[MAX_BUFFER];
    int capacidad;
    int cuenta;
    int in;
    int out;
    pthread_mutex_t mutex_buffer;
    pthread_mutex_t mutex_consola;
    pthread_cond_t no_lleno;
    pthread_cond_t no_vacio;
};

static BufferCompartido buffer;
static int total_a_producir = 0;
static int items_producidos = 0;
static int items_consumidos = 0;
static bool abortar = false;            // se activa si falla la creacion de un hilo
static vector<int> veces_consumido;     // veces que se consumio cada elemento (indice = elemento)

static void imprimir_mensaje(const string& msg) {
    pthread_mutex_lock(&buffer.mutex_consola);
    cout << msg << endl;
    pthread_mutex_unlock(&buffer.mutex_consola);
}

static void* rutina_productor(void* arg) {
    int id = *(int*)arg;

    while (true) {
        pthread_mutex_lock(&buffer.mutex_buffer);

        // Buffer lleno: esperar señal del consumidor
        while (buffer.cuenta == buffer.capacidad && items_producidos < total_a_producir && !abortar) {
            imprimir_mensaje("[Productor " + to_string(id) + "] Buffer LLENO. Esperando espacio...");
            pthread_cond_wait(&buffer.no_lleno, &buffer.mutex_buffer);
        }

        // Ya se produjo todo o hubo un error: terminar y despertar a otros productores en espera
        if (items_producidos >= total_a_producir || abortar) {
            pthread_cond_broadcast(&buffer.no_lleno);
            pthread_mutex_unlock(&buffer.mutex_buffer);
            break;
        }

        // Seccion critica: insercion en el buffer
        int item = ++items_producidos;
        buffer.datos[buffer.in] = item;
        buffer.in = (buffer.in + 1) % buffer.capacidad;
        buffer.cuenta++;

        imprimir_mensaje("[Productor " + to_string(id) + "] PRODUJO elemento: " + to_string(item) +
                         " | Ocupacion: " + to_string(buffer.cuenta) + "/" + to_string(buffer.capacidad));

        pthread_cond_signal(&buffer.no_vacio);
        pthread_mutex_unlock(&buffer.mutex_buffer);

        usleep(ESPERA_PRODUCTOR_US);
    }

    imprimir_mensaje("[Productor " + to_string(id) + "] FINALIZO su ejecucion.");
    return nullptr;
}

static void* rutina_consumidor(void* arg) {
    int id = *(int*)arg;

    while (true) {
        pthread_mutex_lock(&buffer.mutex_buffer);

        // Buffer vacio: esperar señal del productor o el fin de la produccion
        while (buffer.cuenta == 0 && items_consumidos < total_a_producir && !abortar) {
            imprimir_mensaje("[Consumidor " + to_string(id) + "] Buffer VACIO. Esperando elementos...");
            pthread_cond_wait(&buffer.no_vacio, &buffer.mutex_buffer);
        }

        // Buffer vacio y nada mas por consumir: terminar y despertar a otros consumidores
        if (buffer.cuenta == 0) {
            pthread_cond_broadcast(&buffer.no_vacio);
            pthread_mutex_unlock(&buffer.mutex_buffer);
            break;
        }

        // Seccion critica: extraccion del buffer
        int item = buffer.datos[buffer.out];
        buffer.out = (buffer.out + 1) % buffer.capacidad;
        buffer.cuenta--;
        items_consumidos++;
        veces_consumido[item]++;

        imprimir_mensaje("[Consumidor " + to_string(id) + "] CONSUMIO elemento: " + to_string(item) +
                         " | Ocupacion: " + to_string(buffer.cuenta) + "/" + to_string(buffer.capacidad));

        pthread_cond_signal(&buffer.no_lleno);
        pthread_mutex_unlock(&buffer.mutex_buffer);

        usleep(ESPERA_CONSUMIDOR_US);
    }

    imprimir_mensaje("[Consumidor " + to_string(id) + "] FINALIZO su ejecucion.");
    return nullptr;
}

// Inicializa las primitivas en orden. Devuelve cuantas quedaron inicializadas.
static int inicializar_primitivas() {
    if (pthread_mutex_init(&buffer.mutex_buffer, nullptr) != 0) return 0;
    if (pthread_mutex_init(&buffer.mutex_consola, nullptr) != 0) return 1;
    if (pthread_cond_init(&buffer.no_lleno, nullptr) != 0) return 2;
    if (pthread_cond_init(&buffer.no_vacio, nullptr) != 0) return 3;
    return 4;
}

// Destruye solo las primitivas que si llegaron a inicializarse (en orden inverso).
static void destruir_primitivas(int inicializadas) {
    if (inicializadas >= 4) pthread_cond_destroy(&buffer.no_vacio);
    if (inicializadas >= 3) pthread_cond_destroy(&buffer.no_lleno);
    if (inicializadas >= 2) pthread_mutex_destroy(&buffer.mutex_consola);
    if (inicializadas >= 1) pthread_mutex_destroy(&buffer.mutex_buffer);
}

void ejecutar_modulo_concurrencia(int capacidad_buffer, int total_items) {
    if (capacidad_buffer <= 0 || total_items <= 0) {
        cout << "[ERROR] Los parametros deben ser mayores a cero." << endl;
        return;
    }

    // Inicializacion del estado compartido
    total_a_producir = total_items;
    buffer.capacidad = (capacidad_buffer > MAX_BUFFER) ? MAX_BUFFER : capacidad_buffer;
    buffer.cuenta = 0;
    buffer.in = 0;
    buffer.out = 0;
    items_producidos = 0;
    items_consumidos = 0;
    abortar = false;
    try {
        veces_consumido.assign(total_items + 1, 0);
    } catch (const bad_alloc&) {
        cout << "[ERROR] Memoria insuficiente para verificar " << total_items
             << " elementos. Use un valor menor." << endl;
        return;
    }

    int inicializadas = inicializar_primitivas();
    if (inicializadas < 4) {
        cerr << "[ERROR] No se pudieron inicializar los mutex/variables de condicion." << endl;
        destruir_primitivas(inicializadas);
        return;
    }

    pthread_t prods[NUM_PRODUCTORES];
    pthread_t cons[NUM_CONSUMIDORES];
    int ids_prod[NUM_PRODUCTORES];
    int ids_cons[NUM_CONSUMIDORES];

    cout << "\n=======================================================\n";
    cout << "  MODULO II: GESTION DE HILOS Y CONCURRENCIA          \n";
    cout << "  - Capacidad Buffer: " << buffer.capacidad << "\n";
    cout << "  - Total a transferir: " << total_items << "\n";
    cout << "  - Productores: " << NUM_PRODUCTORES << " | Consumidores: " << NUM_CONSUMIDORES << "\n";
    cout << "=======================================================\n";
    if (capacidad_buffer > MAX_BUFFER) {
        cout << "[AVISO] Capacidad solicitada (" << capacidad_buffer
             << ") excede el maximo; se uso " << MAX_BUFFER << ".\n";
    }
    cout << endl;

    // Creacion de hilos concurrentes (con control de errores)
    int prods_creados = 0;
    int cons_creados = 0;
    bool fallo = false;

    for (int i = 0; i < NUM_PRODUCTORES && !fallo; i++) {
        ids_prod[i] = i + 1;
        if (pthread_create(&prods[i], nullptr, rutina_productor, &ids_prod[i]) != 0) {
            cerr << "[ERROR] No se pudo crear el productor " << (i + 1) << endl;
            fallo = true;
        } else {
            prods_creados++;
        }
    }
    for (int i = 0; i < NUM_CONSUMIDORES && !fallo; i++) {
        ids_cons[i] = i + 1;
        if (pthread_create(&cons[i], nullptr, rutina_consumidor, &ids_cons[i]) != 0) {
            cerr << "[ERROR] No se pudo crear el consumidor " << (i + 1) << endl;
            fallo = true;
        } else {
            cons_creados++;
        }
    }

    // Si algo fallo, avisar a los hilos ya creados para que terminen y no queden bloqueados
    if (fallo) {
        pthread_mutex_lock(&buffer.mutex_buffer);
        abortar = true;
        pthread_cond_broadcast(&buffer.no_lleno);
        pthread_cond_broadcast(&buffer.no_vacio);
        pthread_mutex_unlock(&buffer.mutex_buffer);
    }

    // Espera obligatoria de todos los hilos creados
    bool joins_ok = true;
    for (int i = 0; i < prods_creados; i++) {
        if (pthread_join(prods[i], nullptr) != 0) {
            cerr << "[ERROR] pthread_join fallo para el productor " << (i + 1) << endl;
            joins_ok = false;
        }
    }
    for (int i = 0; i < cons_creados; i++) {
        if (pthread_join(cons[i], nullptr) != 0) {
            cerr << "[ERROR] pthread_join fallo para el consumidor " << (i + 1) << endl;
            joins_ok = false;
        }
    }

    // Si no se pudo confirmar que los hilos terminaron, NO se destruyen las primitivas
    if (!joins_ok) {
        cerr << "[ERROR] No se confirmo la finalizacion de todos los hilos; "
                "se omite la destruccion de primitivas y la verificacion." << endl;
        return;
    }

    // Destruccion limpia de primitivas
    destruir_primitivas(inicializadas);

    // Verificacion de integridad: cada elemento debe haberse consumido exactamente una vez
    int perdidos = 0;
    int duplicados = 0;
    for (int i = 1; i <= total_items; i++) {
        if (veces_consumido[i] == 0) perdidos++;
        else if (veces_consumido[i] > 1) duplicados++;
    }

    cout << "\n-------------------------------------------------------\n";
    cout << "[VERIFICACION DE INTEGRIDAD]:\n";
    cout << "  - Elementos producidos: " << items_producidos << "\n";
    cout << "  - Elementos consumidos: " << items_consumidos << "\n";
    cout << "  - Elementos perdidos:   " << perdidos << "\n";
    cout << "  - Elementos duplicados: " << duplicados << "\n";
    if (!fallo && items_producidos == total_a_producir && items_consumidos == total_a_producir
        && perdidos == 0 && duplicados == 0) {
        cout << "  - ESTADO: OK (Consumo exitoso, sin perdidas ni duplicados)\n";
    } else {
        cout << "  - ESTADO: ERROR DE CONSISTENCIA\n";
    }
    cout << "-------------------------------------------------------\n";
}