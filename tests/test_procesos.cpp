#include "procesos.h"

#include <iostream>

using namespace std;

int main() {
    cout << "Prueba con el minimo de hijos (2):" << endl;
    if (!ejecutar_modulo_procesos(2)) {
        return 1;
    }

    cout << "\nPrueba con una cantidad configurable de hijos (4):" << endl;
    if (!ejecutar_modulo_procesos(4)) {
        return 1;
    }

    cout << "\nPrueba de rechazo para una cantidad menor al minimo:" << endl;
    if (ejecutar_modulo_procesos(1)) {
        return 1;
    }

    cout << "Todas las pruebas de procesos finalizaron correctamente." << endl;
    return 0;
}
