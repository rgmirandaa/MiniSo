#include "memoria.h"
#include <iostream>
#include <string>

using namespace std;

const int MAX_BLOQUES = 100;
const int MAX_PAGINAS = 50;

struct Bloque {
    int id;
    int inicio;
    int tamanio;
    bool ocupado;
    string proceso;
};

struct EntradaPagina {
    int numeroPagina;
    int marcoFisico;
    bool asignada;
};

static Bloque memoriaContigua[MAX_BLOQUES];
static int cantBloques = 0;
static int memoriaTotal = 1024;

static EntradaPagina tablaPaginas[MAX_PAGINAS];
static int cantPaginas = 0;
static int tamPagina = 4096;
static int totalMarcos = 8;

void inicializarMemoria(int tamanioTotal) {
    memoriaTotal = tamanioTotal;
    cantBloques = 1;
    memoriaContigua[0] = {1, 0, memoriaTotal, false, "Libre"};
    cout << "[OK] Memoria RAM inicializada con " << memoriaTotal << " KB libres.\n";
}

void fusionarBloquesLibres() {
    for (int i = 0; i < cantBloques - 1; ) {
        if (!memoriaContigua[i].ocupado && !memoriaContigua[i + 1].ocupado) {
            memoriaContigua[i].tamanio += memoriaContigua[i + 1].tamanio;
            for (int j = i + 1; j < cantBloques - 1; j++) {
                memoriaContigua[j] = memoriaContigua[j + 1];
            }
            cantBloques--;
        } else {
            i++;
        }
    }
    for (int i = 0; i < cantBloques; i++) {
        memoriaContigua[i].id = i + 1;
    }
}

void mostrarEstadoMemoria() {
    cout << "\n================ ESTADO DE LA MEMORIA ================\n";
    cout << "ID\tInicio\tTamaño\tEstado\t\tProceso\n";
    cout << "------------------------------------------------------\n";
    int libreTotal = 0;
    int maxContiguo = 0;
    
    for (int i = 0; i < cantBloques; i++) {
        cout << memoriaContigua[i].id << "\t" 
             << memoriaContigua[i].inicio << " KB\t" 
             << memoriaContigua[i].tamanio << " KB\t"
             << (memoriaContigua[i].ocupado ? "[OCUPADO]" : "[LIBRE]  ") << "\t" 
             << memoriaContigua[i].proceso << "\n";
             
        if (!memoriaContigua[i].ocupado) {
            libreTotal += memoriaContigua[i].tamanio;
            if (memoriaContigua[i].tamanio > maxContiguo) {
                maxContiguo = memoriaContigua[i].tamanio;
            }
        }
    }
    cout << "------------------------------------------------------\n";
    cout << "Memoria Libre Total: " << libreTotal << " KB | Máximo Libre Contiguo: " << maxContiguo << " KB\n";
}

bool solicitarMemoriaFirstFit(string idProceso, int tamanioRequerido) {
    int libreTotal = 0;
    for (int i = 0; i < cantBloques; i++) {
        if (!memoriaContigua[i].ocupado) {
            libreTotal += memoriaContigua[i].tamanio;
        }
    }

    for (int i = 0; i < cantBloques; i++) {
        if (!memoriaContigua[i].ocupado && memoriaContigua[i].tamanio >= tamanioRequerido) {
            int tamanioSobrante = memoriaContigua[i].tamanio - tamanioRequerido;
            
            memoriaContigua[i].tamanio = tamanioRequerido;
            memoriaContigua[i].ocupado = true;
            memoriaContigua[i].proceso = idProceso;

            if (tamanioSobrante > 0 && cantBloques < MAX_BLOQUES) {
                for (int j = cantBloques; j > i + 1; j--) {
                    memoriaContigua[j] = memoriaContigua[j - 1];
                }
                memoriaContigua[i + 1] = {
                    0, 
                    memoriaContigua[i].inicio + tamanioRequerido, 
                    tamanioSobrante, 
                    false, 
                    "Libre"
                };
                cantBloques++;
            }
            fusionarBloquesLibres();
            cout << "[EXITO] Proceso " << idProceso << " asignado con " << tamanioRequerido << " KB.\n";
            return true;
        }
    }

    cout << "[ERROR] No se pudo asignar " << tamanioRequerido << " KB al proceso " << idProceso << ".\n";
    if (libreTotal >= tamanioRequerido) {
        cout << ">>> DETECTADA FRAGMENTACION EXTERNA: Hay " << libreTotal 
             << " KB libres en total, pero no hay un bloque contiguo de " << tamanioRequerido << " KB.\n";
    } else {
        cout << ">>> MEMORIA INSUFICIENTE: No hay suficiente espacio en RAM.\n";
    }
    return false;
}

bool liberarMemoria(string idProceso) {
    bool encontrado = false;
    for (int i = 0; i < cantBloques; i++) {
        if (memoriaContigua[i].ocupado && memoriaContigua[i].proceso == idProceso) {
            memoriaContigua[i].ocupado = false;
            memoriaContigua[i].proceso = "Libre";
            encontrado = true;
        }
    }
    if (encontrado) {
        fusionarBloquesLibres();
        cout << "[EXITO] Memoria liberada del proceso " << idProceso << ".\n";
    } else {
        cout << "[ERROR] No se encontro el proceso " << idProceso << ".\n";
    }
    return encontrado;
}

void configurarPaginacion(int tamPag, int numMarcos) {
    tamPagina = tamPag;
    totalMarcos = numMarcos;
    cantPaginas = 0;
    cout << "[OK] Paginacion configurada: Pagina de " << tamPagina << " bytes | Total Marcos: " << totalMarcos << ".\n";
}

void cargarTablaPorDefecto() {
    tamPagina = 4096;
    totalMarcos = 8;
    cantPaginas = 5;
    
    tablaPaginas[0] = {0, 2, true};
    tablaPaginas[1] = {1, 5, true};
    tablaPaginas[2] = {2, 1, true};
    tablaPaginas[3] = {3, 7, true};
    tablaPaginas[4] = {4, -1, false}; // Página no asignada para pruebas de fallo
    
    cout << "[OK] Tabla de paginas por defecto cargada.\n";
}

void mostrarTablaPaginas() {
    if (cantPaginas == 0) cargarTablaPorDefecto();

    cout << "\n=================== TABLA DE PAGINAS ===================\n";
    cout << "No. Pagina\tNo. Marco Asignado\tEstado\n";
    cout << "--------------------------------------------------------\n";
    for (int i = 0; i < cantPaginas; i++) {
        cout << " " << tablaPaginas[i].numeroPagina << "\t\t ";
        if (tablaPaginas[i].asignada) {
            cout << tablaPaginas[i].marcoFisico << "\t\t\tAsignada\n";
        } else {
            cout << "N/A\t\t\t[NO ASIGNADA / FALLO]\n";
        }
    }
}

int traducirDireccion(int direccionLogica) {
    if (cantPaginas == 0) cargarTablaPorDefecto();

    if (direccionLogica < 0) {
        cout << "[ERROR] La direccion logica no puede ser negativa.\n";
        return -1;
    }

    int pagina = direccionLogica / tamPagina;
    int offset = direccionLogica % tamPagina;

    if (pagina >= cantPaginas) {
        cout << "[ERROR] Fallo de Pagina: La direccion " << direccionLogica 
             << " sobrepasa las paginas totales del proceso (Pagina " << pagina << " inexistente).\n";
        return -1;
    }

    if (!tablaPaginas[pagina].asignada) {
        cout << "[ERROR] Fallo de Pagina: La Pagina " << pagina << " esta en estado NO ASIGNADA a la memoria fisica.\n";
        return -1;
    }

    int marco = tablaPaginas[pagina].marcoFisico;
    int direccionFisica = (marco * tamPagina) + offset;

    cout << "\n[TRADUCCION EXITO]:\n";
    cout << "  - Dir. Logica: " << direccionLogica << " bytes\n";
    cout << "  - Pagina: " << pagina << " (Offset/Desplazamiento: " << offset << " bytes)\n";
    cout << "  - Marco Fisico Mapeado: " << marco << "\n";
    cout << "  - Dir. Fisica Calculada: (" << marco << " * " << tamPagina << ") + " << offset << " = " << direccionFisica << " bytes\n";

    return direccionFisica;
}

void menuGestionMemoria() {
    int opcion;
    if (cantBloques == 0) inicializarMemoria(1024);

    do {
        cout << "\n======================================================\n";
        cout << "           MODULO III: GESTION DE MEMORIA             \n";
        cout << "======================================================\n";
        cout << "--- Parte A: Particionamiento Contiguo ---\n";
        cout << "1. Configurar Tamanio Total de RAM\n";
        cout << "2. Solicitar Memoria para Proceso (First-Fit)\n";
        cout << "3. Liberar Memoria de Proceso\n";
        cout << "4. Mostrar Estado de Bloques (Libres/Ocupados)\n";
        cout << "--- Parte B: Paginacion y Traduccion ---\n";
        cout << "5. Configurar Tamanio de Pagina y Marcos\n";
        cout << "6. Mostrar Tabla de Paginas\n";
        cout << "7. Traducir Direccion Logica a Fisica\n";
        cout << "0. Volver al Menu Principal\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int tam;
                cout << "Tamanio total de la RAM (KB): ";
                cin >> tam;
                inicializarMemoria(tam);
                break;
            }
            case 2: {
                string proc;
                int tam;
                cout << "ID del Proceso: ";
                cin >> proc;
                cout << "Tamanio (KB): ";
                cin >> tam;
                solicitarMemoriaFirstFit(proc, tam);
                break;
            }
            case 3: {
                string proc;
                cout << "ID del Proceso a liberar: ";
                cin >> proc;
                liberarMemoria(proc);
                break;
            }
            case 4:
                mostrarEstadoMemoria();
                break;
            case 5: {
                int tp, tm;
                cout << "Tamanio de Pagina en Bytes: ";
                cin >> tp;
                cout << "Cantidad Total de Marcos de Pagina: ";
                cin >> tm;
                configurarPaginacion(tp, tm);
                break;
            }
            case 6:
                mostrarTablaPaginas();
                break;
            case 7: {
                int dir;
                cout << "Ingrese direccion logica en bytes: ";
                cin >> dir;
                traducirDireccion(dir);
                break;
            }
        }
    } while (opcion != 0);
}

