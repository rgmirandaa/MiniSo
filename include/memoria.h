#ifndef MEMORIA_H
#define MEMORIA_H

#include <string>

using std::string;

// Parte A: Particionamiento Contiguo (First-Fit)
void inicializarMemoria(int tamanioTotal);
void fusionarBloquesLibres();
void mostrarEstadoMemoria();

// Parte B: Paginación y Traducción 
void cargarTablaPorDefecto();
void mostrarTablaPaginas();

// Menú Principal de Modulo III
void menuGestionMemoria();

#endif 
