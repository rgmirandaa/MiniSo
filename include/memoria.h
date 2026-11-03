#ifndef MEMORIA_H
#define MEMORIA_H

#include <string>

using std::string;

// Parte A: Particionamiento Contiguo (First-Fit) 
void inicializarMemoria(int tamanioTotal);
void fusionarBloquesLibres();
void mostrarEstadoMemoria();
bool solicitarMemoriaFirstFit(string idProceso, int tamanioRequerido);
bool liberarMemoria(string idProceso);

// Parte B: Paginación y Traducción
void configurarPaginacion(int tamPag, int numMarcos);
void cargarTablaPorDefecto();
void mostrarTablaPaginas();
int traducirDireccion(int direccionLogica);

// Menú Principal del Módulo III
void menuGestionMemoria();

#endif 
