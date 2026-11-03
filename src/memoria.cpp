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

void inicializarMemoria(int tamanioTotal) {
    memoriaTotal = tamanioTotal;
    cantBloques = 1;
    memoriaContigua[0] = {1, 0, memoriaTotal, false, "Libre"};
    cout << "[OK] Memoria RAM inicializada con " << memoriaTotal << " KB libres.\n";
}

void mostrarEstadoMemoria() {
    cout << "\n================ ESTADO DE LA MEMORIA ================\n";
    cout << "ID\tInicio\tTamaño\tEstado\t\tProceso\n";
    cout << "------------------------------------------------------\n";
    for (int i = 0; i < cantBloques; i++) {
        cout << memoriaContigua[i].id << "\t" 
             << memoriaContigua[i].inicio << " KB\t" 
             << memoriaContigua[i].tamanio << " KB\t"
             << (memoriaContigua[i].ocupado ? "[OCUPADO]" : "[LIBRE]  ") << "\t" 
             << memoriaContigua[i].proceso << "\n";
    }
}

void cargarTablaPorDefecto() {
    cantPaginas = 4;
    tablaPaginas[0] = {0, 2, true};
    tablaPaginas[1] = {1, 5, true};
    tablaPaginas[2] = {2, 1, true};
    tablaPaginas[3] = {3, 7, true};
}

void menuGestionMemoria() {
    cout << "Modulo III inicializado.\n";
}
