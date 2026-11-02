#ifndef MEMORIA_H
#define MEMORIA_H

// A. Particionamiento Contiguo (First-Fit)
void firstFit(int blockSize[], int m, const int processSize[], int n);

// B. Paginación y Traducción de Direcciones
void simularPaginacion(const int tablaPaginas[], int nPaginas, int totalMarcos);
int traducirDireccion(int direccionLogica, int tamPagina, const int tablaPaginas[], int nPaginas);

// Menú del Módulo III
void menuGestionMemoria();

#endif 
