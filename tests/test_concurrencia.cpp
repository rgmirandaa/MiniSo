#include "concurrencia.h"
#include <iostream>

using namespace std;

struct Caso {
    int capacidad;
    int elementos;
    const char* descripcion;
    const char* esperado;
};

int main() {
    const Caso casos[] = {
        {5,   20, "Funcionamiento normal",                       "ESTADO: OK"},
        {2,   20, "Buffer pequeno (LLENO/VACIO frecuentes)",     "ESTADO: OK"},
        {1,   10, "Caso extremo: capacidad 1",                   "ESTADO: OK"},
        {10,   1, "Menos elementos que hilos",                   "ESTADO: OK"},
        {300, 15, "Capacidad sobre el maximo (se limita a 256)", "ESTADO: OK + AVISO"},
        {0,   10, "Parametro invalido: capacidad 0",             "[ERROR] parametros"},
        {-3,  10, "Parametro invalido: capacidad negativa",      "[ERROR] parametros"},
        {5,    0, "Parametro invalido: 0 elementos",             "[ERROR] parametros"},
    };

    for (const Caso& c : casos) {
        cout << "\n########## CASO: " << c.descripcion << " ##########\n";
        cout << "Entrada: capacidad=" << c.capacidad << ", elementos=" << c.elementos << "\n";
        cout << "Esperado: " << c.esperado << endl;
        ejecutar_modulo_concurrencia(c.capacidad, c.elementos);
    }
    return 0;
}