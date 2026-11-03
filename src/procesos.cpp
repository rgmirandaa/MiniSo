#include "procesos.h"

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <new>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace {

constexpr int MIN_HIJOS = 2;
constexpr int MAX_HIJOS = 20;

pid_t crear_hijo(int numero) {
    char mensaje[80];
    snprintf(mensaje, sizeof(mensaje), "[Hijo %d] Programa ejecutado con exec().", numero);

    cout.flush();
    cerr.flush();

    const pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    execl("/bin/echo", "echo", mensaje, static_cast<char*>(nullptr));
    const char error[] = "Error: no se pudo ejecutar /bin/echo\n";
    write(STDERR_FILENO, error, sizeof(error) - 1);
    _exit(127);
}

bool esperar_hijo(pid_t pid, int numero) {
    int estado = 0;
    pid_t resultado;
    do {
        resultado = waitpid(pid, &estado, 0);
    } while (resultado == -1 && errno == EINTR);

    if (resultado == -1) {
        const int error = errno;
        cerr << "[Error] waitpid para el hijo " << numero << ": ";
        errno = error;
        perror("waitpid");
        return false;
    }

    if (WIFEXITED(estado)) {
        cout << "  Hijo " << numero << " | PID: " << pid
             << " | Estado: finalizado | Codigo: " << WEXITSTATUS(estado) << '\n';
        return WEXITSTATUS(estado) == 0;
    } else if (WIFSIGNALED(estado)) {
        cout << "  Hijo " << numero << " | PID: " << pid
             << " | Estado: terminado por senal " << WTERMSIG(estado) << '\n';
    } else {
        cout << "  Hijo " << numero << " | PID: " << pid
             << " | Estado: finalizacion no esperada\n";
    }
    return false;
}

}  // namespace

bool ejecutar_modulo_procesos(int cantidad_hijos) {
    if (cantidad_hijos < MIN_HIJOS || cantidad_hijos > MAX_HIJOS) {
        cerr << "[Error] La cantidad de hijos debe estar entre " << MIN_HIJOS
             << " y " << MAX_HIJOS << ".\n";
        return false;
    }

    cout << "\n======================================================\n"
         << "              MODULO I: GESTION DE PROCESOS          \n"
         << "======================================================\n"
         << "  Proceso padre\n"
         << "  PID: " << getpid() << " | PPID: " << getppid() << '\n'
         << "  Hijos solicitados: " << cantidad_hijos << "\n\n"
         << "  Creacion de procesos hijos:\n";
    cout.flush();

    vector<pid_t> hijos;
    try {
        hijos.assign(cantidad_hijos, -1);
    } catch (const bad_alloc&) {
        cerr << "[Error] No hay memoria suficiente para registrar los hijos.\n";
        return false;
    }

    bool correcto = true;
    for (int i = 0; i < cantidad_hijos; ++i) {
        hijos[i] = crear_hijo(i + 1);
        if (hijos[i] == -1) {
            const int error = errno;
            cerr << "[Error] fork para el hijo " << (i + 1) << ": ";
            errno = error;
            perror("fork");
            correcto = false;
        } else {
            cout << "  Hijo " << (i + 1) << " creado"
                 << " | PID: " << hijos[i] << " | PPID: " << getpid() << '\n';
        }
    }

    cout << "\n  Espera y estado de finalizacion:\n";
    for (int i = 0; i < cantidad_hijos; ++i) {
        if (hijos[i] > 0) {
            if (!esperar_hijo(hijos[i], i + 1)) {
                correcto = false;
            }
        }
    }
    cout << "======================================================\n";
    return correcto;
}