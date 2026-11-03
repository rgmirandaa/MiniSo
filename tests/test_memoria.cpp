#include "memoria.h"
#include <iostream>

using namespace std;

struct CasoMemoria {
    const char* descripcion;
    int tipoPrueba;         // 1: Solicitar, 2: Liberar, 3: Traduccion
    const char* idProceso;
    int valor;              // Tamaño KB o Dirección Lógica
    const char* esperado;
};

int main() {
    inicializarMemoria(1024);
    configurarPaginacion(4096, 8);
    cargarTablaPorDefecto();

    const CasoMemoria casos[] = {
        // PARTE A: PARTICIONAMIENTO CONTIGUO (FIRST-FIT) 
        {"Solicitud normal de proceso P1 (400 KB)", 1, "P1", 400, "ESTADO: OK (Bloque asignado)"},
        {"Solicitud normal de proceso P2 (300 KB)", 1, "P2", 300, "ESTADO: OK (Bloque asignado)"},
        {"Solicitud normal de proceso P3 (300 KB)", 1, "P3", 300, "ESTADO: OK (Bloque asignado)"},
        {"Liberacion de proceso P2 para crear hueco", 2, "P2", 0, "ESTADO: OK (Bloque liberado)"},
        {"Prueba de Fragmentacion Externa (P4 pide 400 KB)", 1, "P4", 400, "[ERROR] DETECTADA FRAGMENTACION EXTERNA"},

        // PARTE B: PAGINACIÓN Y TRADUCCIÓN DE DIRECCIONES 
        {"Traduccion 1: Pagina 0 (Dir. Logica 0)", 3, "", 0, "ESTADO: OK -> Pag 0, Offset 0 -> Dir. Fisica 8192"},
        {"Traduccion 2: Pagina 0 (Dir. Logica 1024)", 3, "", 1024, "ESTADO: OK -> Pag 0, Offset 1024 -> Dir. Fisica 9216"},
        {"Traduccion 3: Pagina 1 (Dir. Logica 5000)", 3, "", 5000, "ESTADO: OK -> Pag 1, Offset 904 -> Dir. Fisica 21384"},
        {"Traduccion 4: Pagina 2 (Dir. Logica 8192)", 3, "", 8192, "ESTADO: OK -> Pag 2, Offset 0 -> Dir. Fisica 4096"},
        {"Traduccion 5: Pagina 3 (Dir. Logica 15000)", 3, "", 15000, "ESTADO: OK -> Pag 3, Offset 2712 -> Dir. Fisica 31384"},
        {"Traduccion 6: Fallo de Pagina (Pagina 4 No Asignada)", 3, "", 17000, "[ERROR] Fallo de Pagina: Pagina 4 no asignada"},
        {"Traduccion 7: Direccion fuera de rango", 3, "", 22000, "[ERROR] Fallo de Pagina: Sobrepasa limite"},
        {"Traduccion 8: Direccion logica negativa", 3, "", -500, "[ERROR] Direccion logica invalida"}
    };

    cout << "======================================================\n";
    cout << "                PRUEBAS - MODULO MEMORIA              \n";
    cout << "======================================================\n";

    bool tablaImpresa = false;

    for (const CasoMemoria& c : casos) {
        if (c.tipoPrueba == 3 && !tablaImpresa) {
            cout << "\n======================================================\n";
            cout << "       ESTADO INICIAL DE LA TABLA DE PAGINAS          \n";
            mostrarTablaPaginas();
            cout << "======================================================\n";
            tablaImpresa = true;
        }

        cout << "\n########## CASO: " << c.descripcion << " ##########\n";
        cout << "Esperado: " << c.esperado << endl;

        if (c.tipoPrueba == 1) {
            cout << "Ejecutando: solicitarMemoriaFirstFit(\"" << c.idProceso << "\", " << c.valor << " KB)\n";
            solicitarMemoriaFirstFit(c.idProceso, c.valor);
            mostrarEstadoMemoria();
        } else if (c.tipoPrueba == 2) {
            cout << "Ejecutando: liberarMemoria(\"" << c.idProceso << "\")\n";
            liberarMemoria(c.idProceso);
            mostrarEstadoMemoria();
        } else if (c.tipoPrueba == 3) {
            cout << "Ejecutando: traducirDireccion(" << c.valor << " bytes)\n";
            traducirDireccion(c.valor);
        }
    }

    return 0;
}
