// define.h

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#ifndef CLOCK_THREAD_H
#define CLOCK_THREAD_H

#define MEM_SIZE 16777216   // Tamaño total de la memoria física (2^24 bytes)
#define WORD_SIZE 4         // Tamaño de una palabra en bytes
#define PageTable 4194304  // Posicion inicial pagina de tablas 2^22

#include <pthread.h>

//Estructuras

struct MemoryManagement {
    int code;   // Puntero a la dirección virtual de comienzo del segmento de código.
    int data;   // Puntero a la dirección virtual de comienzo del segmento de datos.
    int pgb;    // Puntero a la dirección física de la correspondiente tabla de páginas.
};

struct PCB {
    int PID;    //Numero de proceso
    char state[15]; //Estado del proceso
    int remainingTime; //Tiempo de duracion de proceso
    int quantum;    //Tiempo maximo del proceso si hay procesos esperando
    struct MemoryManagement mm; //Estructura con campos de la memoria (Parte 3)
    struct PCB* next;   //Siguiente proceso 
};

struct ProcessQueue {
    struct PCB* first;
    struct PCB* last;
};

struct Thread {
    int tid;    //Numero de hilo
    struct PCB* process; //Proceso asignado al hilo
    //Parte 3
    // struct MMU* mmu; //Traduccion las direcciones virtuales a direccciones fisicas
    // int ptbr;  // Registro apuntador a la tabla de páginas (PTBR)
    // int RI; //Instruccion actual
    // int PC; //DIreccion de la proxima instruccion
};

struct Core {
    int core_id;    //Numero de core     
    struct Thread* threads; //Hilos dentro del core
};

struct CPU {
    int cpu_id;    //Numero de cpu
    struct Core* cores;   //Cores dentro del cpu
};

struct TLBEntry {
    int virtualPageNumber;  // Número de página virtual
    int frameNumber;        // Número de marco en memoria física
    int valid;              // Bit de validez
};

struct MMU {
    struct TLBEntry* tlb; //Tabla de paginas
};


//Variables

extern pthread_mutex_t mutex;
extern pthread_cond_t cond1, cond2;
extern int done;

extern struct ProcessQueue myQueue;
extern int freeThreads;
extern int waitingProcess;
extern int processActivated;
extern int max_process;


extern int num_cpus;    
extern int num_cores;   
extern int num_threads; 
extern int type_scheduler;

extern struct CPU* CPUsMachine; 

extern int randPID;

extern int physicalMemory[];

//Funciones

void *clock_thread();
void *timer1(void *args);
void *timer2(void *args);
void scheduler_RobinRound();
void scheduler_FIFO();
void process_generator_loader();
void addPCB(struct ProcessQueue* myQueue, struct PCB* pcb);
void deletePCB(struct ProcessQueue* myQueue, struct PCB* pcb);
void processGoLast(struct ProcessQueue* myQueue, struct PCB* pcb);


#endif  // define_h
