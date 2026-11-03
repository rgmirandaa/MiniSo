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
            if (memoriaContigua[i].tamanio > maxContiguo) maxContiguo = memoriaContigua[i].tamanio;
        }
    }
    cout << "------------------------------------------------------\n";
    cout << "Memoria Libre Total: " << libreTotal << " KB | Máximo Libre Contiguo: " << maxContiguo << " KB\n";
}

bool solicitarMemoriaFirstFit(string idProceso, int tamanioRequerido) {
    int libreTotal = 0;
    for (int i = 0; i < cantBloques; i++) {
        if (!memoriaContigua[i].ocupado) libreTotal += memoriaContigua[i].tamanio;
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
                memoriaContigua[i + 1] = {0, memoriaContigua[i].inicio + tamanioRequerido, tamanioSobrante, false, "Libre"};
                cantBloques++;
            }
            fusionarBloquesLibres();
            cout << "[EXITO] Proceso " << idProceso << " asignado con " << tamanioRequerido << " KB.\n";
            return true;
        }
    }

    cout << "[ERROR] No se pudo asignar " << tamanioRequerido << " KB al proceso " << idProceso << ".\n";
    if (libreTotal >= tamanioRequerido) {
        cout << ">>> DETECTADA FRAGMENTACION EXTERNA: Hay " << libreTotal << " KB libres pero no contiguos.\n";
    } else {
        cout << ">>> MEMORIA INSUFFICIENTE en RAM.\n";
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

void cargarTablaPorDefecto() {
    cantPaginas = 4;
    tablaPaginas[0] = {0, 2, true};
    tablaPaginas[1] = {1, 5, true};
    tablaPaginas[2] = {2, 1, true};
    tablaPaginas[3] = {3, 7, true};
}

void mostrarTablaPaginas() {
    if (cantPaginas == 0) cargarTablaPorDefecto();
    cout << "\n=================== TABLA DE PAGINAS ===================\n";
    cout << "No. Pagina\tNo. Marco Asignado\tEstado\n";
    cout << "--------------------------------------------------------\n";
    for (int i = 0; i < cantPaginas; i++) {
        cout << " " << tablaPaginas[i].numeroPagina << "\t\t " << tablaPaginas[i].marcoFisico << "\t\t\tAsignada\n";
    }
}

void menuGestionMemoria() {
    cout << "Funciones basicas de First-Fit e integracion inicial listas.\n";
}
