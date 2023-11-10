#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Estructura PCB
struct PCB {
    int PID;
    // Otros campos relacionados con el PCB
    struct PCB* siguiente;
};

// Estructura ProcessQueue
struct ProcessQueue {
    struct PCB* first;
    struct PCB* last;
};

void initializeProcessQueue(struct ProcessQueue* lista){
    lista->first = NULL;
    lista->last = NULL;
}


// Estructura para representar la máquina (CPUs, cores, hilos)
typedef struct {
    int num_cpus;    // Número de CPUs en la máquina
    int num_cores;   // Número de cores por CPU
    int num_threads; // Número de hilos por core
    // Otras propiedades de la máquina a medida que se amplía el simulador
} Machine;

// Función para simular el Clock
void *clock_thread(void *args) {
    // Lógica para avanzar la estructura de CPUs, cores e hilos hardware
    // y señalar al Timer para que funcione.
    while (1) {
        // Avanzar la estructura de hardware
        // Señalar al Timer para que funcione
        sleep(1); // Simula un ciclo de ejecución
    }
    return NULL;
}

// Función para simular el Timer
void *timer_thread(void *args) {
    // Lógica para generar una señal cada cierto periodo de tiempo
    while (1) {
        // Generar una señal (interrupción) periódica
        // Avisar al Scheduler/Dispatcher y otras funciones del sistema
        sleep(1); // Simula el tiempo entre señales
    }
    return NULL;
}

// Función para simular el Process Generator
void *process_generator_thread(void *args) {
    // Lógica para generar procesos (PCBs) de manera aleatoria
    while (1) {
        // Generar procesos aleatorios y agregarlos a la cola de procesos
        sleep(2); // Simula el tiempo entre generaciones de procesos
    }
    return NULL;
}

void addPCB(struct ProcessQueue* lista, struct PCB* pcb){
    if(lista->first==NULL){
        lista->first=pcb;
        lista->last=pcb;
    }else{
        lista->last->siguiente=pcb;
        lista->last=pcb;
        }
}

int main(int argc, char *argv[]) {

// Inicializar una cola de procesos con capacidad para 10 PCBs
    // Crear una cola de procesos vacía
    struct ProcessQueue myQueue;
    initializeProcessQueue(&myQueue);

    // Añadir dos nuevos PCB a la cola
    struct PCB* pcb1 = (struct PCB*)malloc(sizeof(struct PCB));
    pcb1->PID = 1;
    pcb1->siguiente = NULL;
    addPCB(&myQueue, pcb1);

    struct PCB* pcb2 = (struct PCB*)malloc(sizeof(struct PCB));
    pcb2->PID = 2;
    pcb2->siguiente = NULL;
    addPCB(&myQueue, pcb2);

    // Puedes seguir añadiendo más PCBs según sea necesario

    // Imprimir el contenido de la cola
    struct PCB* current = myQueue.first;
    while (current != NULL) {
        printf("PID: %d\n", current->PID);
        current = current->siguiente;
    }

    // Liberar memoria asignada para PCBs
    current = myQueue.first;
    while (current != NULL) {
        struct PCB* temp = current;
        current = current->siguiente;
        free(temp);
    }

    return 0;

    // Parsear las opciones de configuración si es necesario

    // Inicializar las estructuras de datos que se van a utilizar
    // Machine machine;
    // Inicializar otras estructuras de datos a medida que se amplía el simulador

    // // Crear hilos para cada subsistema
    // pthread_t clock_tid, timer_tid, process_generator_tid;
    // pthread_create(&clock_tid, NULL, clock_thread, NULL);
    // pthread_create(&timer_tid, NULL, timer_thread, NULL);
    // pthread_create(&process_generator_tid, NULL, process_generator_thread, NULL);

    // // Esperar a que los hilos terminen (esto no debería suceder en un kernel real)
    // pthread_join(clock_tid, NULL);
    // pthread_join(timer_tid, NULL);
    // pthread_join(process_generator_tid, NULL);

    return 0;
}
