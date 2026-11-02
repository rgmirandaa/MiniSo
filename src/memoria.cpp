#include "../include/memoria.h"
#include <iostream>

using namespace std;

// Algoritmo First-Fit
void firstFit(int blockSize[], int m, const int processSize[], int n) {
    int* allocation = new int[n];
    for (int i = 0; i < n; ++i) allocation[i] = -1;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (blockSize[j] >= processSize[i]) {
                allocation[i] = j;
                blockSize[j] -= processSize[i];
                break;
            }
        }
    }

    cout << "\nNo. Proceso\tTamano Proceso\tNo. Bloque\n";
    for (int i = 0; i < n; ++i) {
        cout << " " << (i + 1) << "\t\t" << processSize[i] << "\t\t";
        if (allocation[i] != -1) cout << (allocation[i] + 1);
        else cout << "No Asignado";
        cout << "\n";
    }

    delete[] allocation;
}

// Simulación de Tabla de Páginas
void simularPaginacion(const int tablaPaginas[], int nPaginas, int totalMarcos) {
    cout << "\nNo. Pagina\tNo. Marco Asignado\tEstado\n";
    for (int i = 0; i < nPaginas; ++i) {
        cout << " " << i << "\t\t";
        if (tablaPaginas[i] != -1 && tablaPaginas[i] < totalMarcos) {
            cout << tablaPaginas[i] << "\t\t\tAsignada\n";
        } else {
            cout << "No Asignado\t\tFallo de Pagina\n";
        }
    }
}

// Traducción de dirección lógica a física
int traducirDireccion(int direccionLogica, int tamPagina, const int tablaPaginas[], int nPaginas) {
    int pagina = direccionLogica / tamPagina;
    int desplazamiento = direccionLogica % tamPagina;

    if (pagina < 0 || pagina >= nPaginas || tablaPaginas[pagina] == -1) {
        cout << "\n[ERROR] Fallo de pagina o direccion logica invalida.\n";
        return -1;
    }

    int marco = tablaPaginas[pagina];
    int direccionFisica = (marco * tamPagina) + desplazamiento;

    cout << "\n[TRADUCCION]: Dir. Logica " << direccionLogica 
         << " -> Pagina " << pagina << " (Offset: " << desplazamiento << ")"
         << " -> Marco " << marco << " -> Dir. Fisica: " << direccionFisica << "\n";

    return direccionFisica;
}

// Menú del Módulo III
void menuGestionMemoria() {
    int opcion;
    do {
        cout << "\n--- MODULO III: GESTION DE MEMORIA ---\n";
        cout << "1. Ejecutar Particionamiento (First-Fit)\n";
        cout << "2. Mostrar Tabla de Paginas\n";
        cout << "3. Traducir Direccion Logica a Fisica\n";
        cout << "0. Volver al Menu Principal\n";
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            int blockSize[] = {100, 500, 200, 300, 600};
            int processSize[] = {212, 417, 112, 301};
            firstFit(blockSize, 5, processSize, 4);
        } else if (opcion == 2) {
            int tablaPaginas[] = {2, 5, 1, 7};
            simularPaginacion(tablaPaginas, 4, 8);
        } else if (opcion == 3) {
            int tablaPaginas[] = {2, 5, 1, 7};
            int dirLogica;
            cout << "Ingrese direccion logica (ej. 5000): ";
            cin >> dirLogica;
            traducirDireccion(dirLogica, 4096, tablaPaginas, 4);
        }
    } while (opcion != 0);
}