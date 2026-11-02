#include "concurrencia.h"
#include <iostream>
#include <pthread.h>

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

void ejecutar_modulo_concurrencia(int capacidad_buffer, int total_items) {
    cout<<"[Modulo II] Estructura inicializada para "<<total_items<<
            " items con buffer de "<<capacidad_buffer<<endl;
}