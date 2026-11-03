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
    if (tamPag <= 0 || numMarcos <= 0) {
        cout << "[ERROR] El tamaño de página y los marcos deben ser mayores a 0.\n";
        return;
    }
    tamPagina = tamPag;
    totalMarcos = numMarcos;
    cantPaginas = (numMarcos < MAX_PAGINAS) ? numMarcos : MAX_PAGINAS;
    for (int i = 0; i < cantPaginas; i++) {
        if (i < cantPaginas - 1) {
            tablaPaginas[i] = {i, (i * 2 + 1) % totalMarcos, true};
        } else {
            tablaPaginas[i] = {i, -1, false};
        }
    }
    cout << "[OK] Paginación configurada: Página de " << tamPagina << " bytes | Total Marcos: " << totalMarcos << ".\n";
}

void cargarTablaPorDefecto() {
    if (cantPaginas > 0) return; 

    tamPagina = 4096;
    totalMarcos = 8;
    cantPaginas = 5;
    
    tablaPaginas[0] = {0, 2, true};
    tablaPaginas[1] = {1, 5, true};
    tablaPaginas[2] = {2, 1, true};
    tablaPaginas[3] = {3, 7, true};
    tablaPaginas[4] = {4, -1, false}; 
}

void mostrarTablaPaginas() {
    if (cantPaginas == 0) cargarTablaPorDefecto();

    cout << "\n=================== TABLA DE PAGINAS ===================\n";
    cout << "Parámetros Activos: Tamaño Página = " << tamPagina << " bytes | Marcos Totales = " << totalMarcos << "\n";
    cout << "--------------------------------------------------------\n";
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
        cout << "[ERROR] La dirección lógica no puede ser negativa.\n";
        return -1;
    }

    int pagina = direccionLogica / tamPagina;
    int offset = direccionLogica % tamPagina;

    if (pagina >= cantPaginas) {
        cout << "[ERROR] Fallo de Página: La dirección " << direccionLogica 
             << " sobrepasa las páginas totales (" << cantPaginas << " páginas disponibles).\n";
        return -1;
    }

    if (!tablaPaginas[pagina].asignada) {
        cout << "[ERROR] Fallo de Página: La Página " << pagina << " está en estado NO ASIGNADA a la memoria física.\n";
        return -1;
    }

    int marco = tablaPaginas[pagina].marcoFisico;
    int direccionFisica = (marco * tamPagina) + offset;

    cout << "\n[TRADUCCION EXITO]:\n";
    cout << "  - Dir. Lógica: " << direccionLogica << " bytes\n";
    cout << "  - Página: " << pagina << " (Offset/Desplazamiento: " << offset << " bytes)\n";
    cout << "  - Marco Físico Mapeado: " << marco << "\n";
    cout << "  - Dir. Física Calculada: (" << marco << " * " << tamPagina << ") + " << offset << " = " << direccionFisica << " bytes\n";

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
        cout << "1. Configurar Tamaño Total de RAM\n";
        cout << "2. Solicitar Memoria para Proceso (First-Fit)\n";
        cout << "3. Liberar Memoria de Proceso\n";
        cout << "4. Mostrar Estado de Bloques (Libres/Ocupados)\n";
        cout << "--- Parte B: Paginación y Traducción ---\n";
        cout << "5. Configurar Tamaño de Página y Marcos\n";
        cout << "6. Mostrar Tabla de Páginas\n";
        cout << "7. Traducir Dirección Lógica a Física\n";
        cout << "0. Volver al Menú Principal\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int tam;
                cout << "Tamaño total de la RAM (KB): ";
                cin >> tam;
                inicializarMemoria(tam);
                break;
            }
            case 2: {
                string proc;
                int tam;
                cout << "ID del Proceso (ej. P1): ";
                cin >> proc;
                cout << "Tamaño (KB): ";
                cin >> tam;
                solicitarMemoriaFirstFit(proc, tam);
                break;
            }
            case 3: {
                string proc;
                cout << "ID del Proceso a liberar (ej. P1): ";
                cin >> proc;
                liberarMemoria(proc);
                break;
            }
            case 4:
                mostrarEstadoMemoria();
                break;
            case 5: {
                int tp, tm;
                cout << "Tamaño de Página en Bytes (ej. 2048): ";
                cin >> tp;
                cout << "Cantidad Total de Marcos de Página (ej. 4): ";
                cin >> tm;
                configurarPaginacion(tp, tm);
                break;
            }
            case 6:
                mostrarTablaPaginas();
                break;
            case 7: {
                int dir;
                cout << "Ingrese dirección lógica en bytes (ej. 5000): ";
                cin >> dir;
                traducirDireccion(dir);
                break;
            }
        }
    } while (opcion != 0);
}
