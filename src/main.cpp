#include "concurrencia.h"
#include "memoria.h"
#include "procesos.h"

#include <iostream>
#include <limits>

using namespace std;

namespace {

bool leer_entero(const char* mensaje, int& valor) {
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            return true;
        }
        if (cin.eof()) {
            return false;
        }

        cout << "Entrada no valida. Ingrese un numero entero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

}  // namespace

int main() {
    while (true) {
        cout << "\n======================================================\n"
             << "             MINISO: SISTEMA OPERATIVO               \n"
             << "======================================================\n"
             << "  [1] Gestion de procesos\n"
             << "  [2] Concurrencia y sincronizacion\n"
             << "  [3] Simulacion de memoria\n"
             << "  [0] Salir\n"
             << "------------------------------------------------------\n";

        int opcion;
        if (!leer_entero("  Seleccione una opcion: ", opcion)) {
            cout << "\nFin de la entrada. Cerrando MiniSO.\n";
            return 0;
        }

        switch (opcion) {
            case 1: {
                int cantidad_hijos;
                if (!leer_entero("\n  Cantidad de procesos hijos (2-20): ", cantidad_hijos)) {
                    cout << "\nFin de la entrada. Cerrando MiniSO.\n";
                    return 0;
                }
                ejecutar_modulo_procesos(cantidad_hijos);
                break;
            }
            case 2: {
                int capacidad;
                int elementos;
                if (!leer_entero("\n  Capacidad del buffer: ", capacidad)
                    || !leer_entero("  Cantidad de elementos: ", elementos)) {
                    cout << "\nFin de la entrada. Cerrando MiniSO.\n";
                    return 0;
                }
                ejecutar_modulo_concurrencia(capacidad, elementos);
                break;
            }
            case 3:
                menuGestionMemoria();
                break;
            case 0:
                cout << "Cerrando MiniSO.\n";
                return 0;
            default:
                cout << "Opcion no valida.\n";
                break;
        }
    }
}