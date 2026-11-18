# MiniSO

Aplicación de consola en C++ que demuestra gestión de procesos, concurrencia y simulación de memoria en Linux.

## Requisitos

- Ubuntu o una distribución Linux compatible.
- `g++` con soporte para C++17.
- GNU Make.
- Bibliotecas POSIX para procesos e hilos (`fork`, `exec`, `waitpid` y pthreads).

En Ubuntu, instala las herramientas necesarias con:

```bash
sudo apt update
sudo apt install build-essential
```

## Compilación y pruebas

Desde la carpeta del proyecto:

```bash
make
make test
```

El ejecutable se genera en `build/miniso`. Para eliminar los binarios:

```bash
make clean
```

## Estructura

```text
MiniSO/
├── src/       # Implementación de los módulos y menú principal
├── include/   # Interfaces de los módulos
├── tests/     # Programas de prueba por módulo
├── Makefile   # Compilación y ejecución de pruebas
└── .gitignore
```

## Uso

Inicia la aplicación con:

```bash
./build/miniso
```

En el menú principal selecciona:

1. **Gestión de procesos:** ingresa entre 2 y 20 procesos hijos. Se muestran sus PID, PPID y estado de finalización.
2. **Concurrencia:** ingresa la capacidad del búfer y la cantidad de elementos para el escenario productor-consumidor.
3. **Memoria:** abre el menú de asignación contigua First-Fit y simulación de paginación.
0. **Salir:** cierra la aplicación.

La gestión de memoria es una simulación; no modifica la memoria real del sistema operativo.

## Videos de demostración

A continuación se presentan las sustentaciones individuales de cada módulo desarrollado para **MiniSO**:

| Estudiante | Módulo Sustentado | Enlace de Video |
| :--- | :--- | :--- |
| **[Roberto Gabriel Miranda Antezana]** | Módulo I: Gestión de Procesos | [Ver video de demostración](https://www.youtube.com/watch?v=LINK_INTEGRANTE_1) |
| **[Jhon Andrioly Zenteno Sosa]** | Módulo II: Concurrencia y Sincronización | [Ver video de demostración](https://www.youtube.com/watch?v=LINK_INTEGRANTE_2) |
| **[Alejandra del Rosario Ramos Mamani]** | Módulo III: Gestión de Memoria | [Ver video de demostración](https://www.youtube.com/watch?v=TU_LINK_AQUI) |
