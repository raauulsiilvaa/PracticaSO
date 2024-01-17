// define.h

#ifndef CLOCK_THREAD_H
#define CLOCK_THREAD_H

#define MEM_SIZE 16777216   // Tamaño total de la memoria física (2^24 bytes)
#define WORD_SIZE 4         // Tamaño de una palabra en bytes
#define PageTable 4194304  // Posicion inicial pagina de tablas 2^22

#include <pthread.h>

//Estructuras 

struct MemoryManagement {
    void* code;   // Puntero a la dirección virtual de comienzo del segmento de código.
    void* data;   // Puntero a la dirección virtual de comienzo del segmento de datos.
    void* pgb;    // Puntero a la dirección física de la correspondiente tabla de páginas.
};

struct PCB {
    int PID;
    char state[15];
    int remainingTime;
    int quantum;
   // struct MemoryManagement mm;
    struct PCB* next;
};

struct ProcessQueue {
    struct PCB* first;
    struct PCB* last;
};

struct Thread {
    int tid;
    struct PCB* process;
    // struct MMU* mmu; //Traduccion las direcciones virtuales a direccciones fisicas
    // void* ptbr;  // Registro apuntador a la tabla de páginas (PTBR)
    // int RI; //Instruccion actual
    // void* PC; //DIreccion de la proxima instruccion
};

struct Core {
    int core_id;        
    struct Thread* threads;
};

struct CPU {
    int cpu_id;    
    struct Core* cores;   
};

struct TLBEntry {
    int virtualPageNumber;  // Número de página virtual
    int frameNumber;        // Número de marco en memoria física
    int valid;              // Bit de validez
};

struct MMU {
    struct TLBEntry* tlb;
};

struct VirtualMemory {
    char *memory;  // Puntero a la memoria virtual
    size_t size;            // Tamaño total de la memoria
};

//Variables

extern pthread_mutex_t mutex;
extern pthread_cond_t cond1, cond2;
extern int done;

extern struct ProcessQueue myQueue;
extern int freeThreads;
extern int waitingProcess;
extern int processActivated;

extern int num_cpus;    
extern int num_cores;   
extern int num_threads; 
extern struct CPU* CPUsMachine; 

extern int randPID;

extern int physicalMemory[];

//Funciones

void *clock_thread();
void *timer1(void *args);
void *timer2(void *args);
void scheduler();
void process_generator_thread();
void addPCB(struct ProcessQueue* myQueue, struct PCB* pcb);
void deletePCB(struct ProcessQueue* myQueue, struct PCB* pcb);
void processGoLast(struct ProcessQueue* myQueue, struct PCB* pcb);


#endif  // define_h
