#include "concurrencia.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define MAX_BUFFER 256

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

static void imprimir_mensaje(const string& msg) {
    pthread_mutex_lock(&buffer.mutex_consola);
    cout << msg << endl;
    pthread_mutex_unlock(&buffer.mutex_consola);
}

static void* rutina_productor(void* arg) {
    long id = (long)arg;

    while (true) {
        pthread_mutex_lock(&buffer.mutex_buffer);

        if (items_producidos >= total_a_producir) {
            pthread_mutex_unlock(&buffer.mutex_buffer);
            break;
        }

        // Buffer lleno: esperar señal del consumidor
        while (buffer.cuenta == buffer.capacidad && items_producidos < total_a_producir) {
            imprimir_mensaje("[Productor " + to_string(id) + "] Buffer LLENO. Esperando espacio...");
            pthread_cond_wait(&buffer.no_lleno, &buffer.mutex_buffer);
        }

        if (items_producidos >= total_a_producir) {
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

        usleep(70000); // 70 ms
    }
    pthread_exit(nullptr);
}

static void* rutina_consumidor(void* arg) {
    long id = (long)arg;

    while (true) {
        pthread_mutex_lock(&buffer.mutex_buffer);

        // Buffer vacio: esperar señal del productor o terminar
        while (buffer.cuenta == 0) {
            if (items_consumidos >= total_a_producir) {
                pthread_cond_broadcast(&buffer.no_vacio);
                pthread_mutex_unlock(&buffer.mutex_buffer);
                pthread_exit(nullptr);
            }
            imprimir_mensaje("[Consumidor " + to_string(id) + "] Buffer VACIO. Esperando elementos...");
            pthread_cond_wait(&buffer.no_vacio, &buffer.mutex_buffer);
        }

        // Seccion critica: extraccion del buffer
        int item = buffer.datos[buffer.out];
        buffer.out = (buffer.out + 1) % buffer.capacidad;
        buffer.cuenta--;
        items_consumidos++;

        imprimir_mensaje("[Consumidor " + to_string(id) + "] CONSUMIO elemento: " + to_string(item) +
                         " | Ocupacion: " + to_string(buffer.cuenta) + "/" + to_string(buffer.capacidad));

        pthread_cond_signal(&buffer.no_lleno);
        pthread_mutex_unlock(&buffer.mutex_buffer);

        usleep(110000); // 110 ms
    }
}

void ejecutar_modulo_concurrencia(int capacidad_buffer, int total_items) {
    if (capacidad_buffer <= 0 || total_items <= 0) {
        cout << "[ERROR] Los parametros deben ser mayores a cero." << endl;
        return;
    }

    total_a_producir = total_items;
    buffer.capacidad = (capacidad_buffer > MAX_BUFFER) ? MAX_BUFFER : capacidad_buffer;
    buffer.cuenta = 0;
    buffer.in = 0;
    buffer.out = 0;
    items_producidos = 0;
    items_consumidos = 0;

    pthread_mutex_init(&buffer.mutex_buffer, nullptr);
    pthread_mutex_init(&buffer.mutex_consola, nullptr);
    pthread_cond_init(&buffer.no_lleno, nullptr);
    pthread_cond_init(&buffer.no_vacio, nullptr);

    const int NUM_PRODUCTORES = 2;
    const int NUM_CONSUMIDORES = 2;
    pthread_t prods[NUM_PRODUCTORES];
    pthread_t cons[NUM_CONSUMIDORES];

    cout << "\n=======================================================\n";
    cout << "  MODULO II: GESTION DE HILOS Y CONCURRENCIA          \n";
    cout << "  - Capacidad Buffer: " << capacidad_buffer << "\n";
    cout << "  - Total a transferir: " << total_items << "\n";
    cout << "  - Productores: " << NUM_PRODUCTORES << " | Consumidores: " << NUM_CONSUMIDORES << "\n";
    cout << "=======================================================\n\n";

    // Creacion de hilos concurrentes
    for (long i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_create(&prods[i], nullptr, rutina_productor, (void*)(i + 1));
    }
    for (long i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_create(&cons[i], nullptr, rutina_consumidor, (void*)(i + 1));
    }

    // Espera obligatoria
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(prods[i], nullptr);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(cons[i], nullptr);
    }

    // Destruccion limpia de primitivas
    pthread_mutex_destroy(&buffer.mutex_buffer);
    pthread_mutex_destroy(&buffer.mutex_consola);
    pthread_cond_destroy(&buffer.no_lleno);
    pthread_cond_destroy(&buffer.no_vacio);

    cout << "\n-------------------------------------------------------\n";
    cout << "[VERIFICACION DE INTEGRIDAD]:\n";
    cout << "  - Elementos producidos: " << items_producidos << "\n";
    cout << "  - Elementos consumidos: " << items_consumidos << "\n";
    if (items_producidos == total_a_producir && items_consumidos == total_a_producir) {
        cout << "  - ESTADO: OK (Consumo exitoso, sin perdidas ni duplicados)\n";
    } else {
        cout << "  - ESTADO: ERROR DE CONSISTENCIA\n";
    }
    cout << "-------------------------------------------------------\n";
}