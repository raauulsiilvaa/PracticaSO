#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define MEM_SIZE 16777216   // Tamaño total de la memoria física (2^24 bytes)
#define WORD_SIZE 4         // Tamaño de una palabra en bytes
#define KERNEL_SIZE 1048576 // Tamaño reservado para el Kernel (1 MB)


//Estrucuturas Necesarias

struct MemoryManagement {
    void* code;   // Puntero a la dirección virtual de comienzo del segmento de código.
    void* data;   // Puntero a la dirección virtual de comienzo del segmento de datos.
    void* pgb;    // Puntero a la dirección física de la correspondiente tabla de páginas.
};

struct PCB {
    int PID;
    char state[15];
    int remainingTime;
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

struct PhysicalMemory {
    int memory[MEM_SIZE];  // Array memoria física
};

struct VirtualMemory {
    char *memory;  // Puntero a la memoria virtual
    size_t size;            // Tamaño total de la memoria
};


//Variables necesarias

pthread_mutex_t mutex;
pthread_cond_t cond1, cond2;
int done = 0;

struct ProcessQueue myQueue;

int num_cpus;    
int num_cores;   
int num_threads; 
struct CPU* CPUsMachine; 

int randPID = 0;

struct PhysicalMemory physicalMemory;

void initializeProcessQueue(struct ProcessQueue* myQueue){
    myQueue->first = NULL;
    myQueue->last = NULL;
}

void addPCB(struct ProcessQueue* myQueue, struct PCB* pcb){    

    if(myQueue->first==NULL){
        myQueue->first=pcb;
        myQueue->last=pcb;
    }else{
        myQueue->last->next=pcb;
        myQueue->last=pcb;
        }

    myQueue->last->next = myQueue->first;
}

void deletePCB(struct ProcessQueue* myQueue, struct PCB* pcb){    

    if(myQueue->first != NULL && pcb != NULL){
        if (pcb->next == pcb){
            myQueue->first = NULL;
            myQueue->last = NULL;

        }else if (myQueue->first == pcb){
            myQueue->first = pcb->next;
            myQueue->last->next = myQueue->first;

        }else {
            struct PCB *aux = myQueue->first;
            while (aux->next != pcb){
                aux = aux->next;
            }
            if (aux->next == myQueue->last){
                aux->next = myQueue->first;
                myQueue->last = aux;
                printf("ID: %d", myQueue->last->PID);
            }else{
                aux->next = pcb->next;
            }
        }
        pcb->next = NULL;
    }
    else{
        printf("Delete incorrecto");
    }

}

 // Funciones necesarias

struct CPU* initializeMachine() {
    // Asignar memoria para un array de CPUs
    CPUsMachine = (struct CPU*)malloc(num_cpus * sizeof(struct CPU));

    // Inicializar cada CPU en el array
    for (int i = 0; i < num_cpus; i++) {
        CPUsMachine[i].cpu_id = i;
        CPUsMachine[i].cores = (struct Core*)malloc(num_cores * sizeof(struct Core));

        // Inicializar cada núcleo en la CPU
        for (int j = 0; j < num_cores; j++) {
            CPUsMachine[i].cores[j].core_id = i * num_cores + j;
            CPUsMachine[i].cores[j].threads = (struct Thread*)malloc(num_threads * sizeof(struct Thread));

            // Inicializar cada hilo en el núcleo
            for (int k = 0; k < num_threads; k++) {
                CPUsMachine[i].cores[j].threads[k].tid = i * num_cores * num_threads + j * num_threads + k;

                // Asignar NULL al puntero del PCB en cada hilo
                CPUsMachine[i].cores[j].threads[k].process= (struct PCB*)malloc(sizeof(struct PCB));
                CPUsMachine[i].cores[j].threads[k].process = NULL;
            }
        }
    }
    
    return CPUsMachine;
}

// Función para liberar la memoria asignada para la máquina
void freeMachine(struct CPU* CPUsMachine, int num_cpus, int num_cores) {
    // Liberar la memoria para hilos (threads), núcleos (cores), y CPUs:

    myQueue.first = NULL;
    myQueue.last = NULL;

    for (int i = 0; i < num_cpus; i++) {
        for (int j = 0; j < num_cores; j++) {
            free(CPUsMachine[i].cores[j].threads);
        }
        free(CPUsMachine[i].cores);
    }
    //Liberar de la memoria para la lista enlazada de PCBs:
    struct PCB* aux1 = myQueue.first;
    struct PCB* aux2;

    while (aux1 != NULL) {
        aux2 = aux1->next;
        free(aux1);
        aux1 = aux2;
    }

}

// Función para imprimir la información de la máquina
void printMachineInfo(struct CPU* CPUsMachine, int num_cpus, int num_cores, int num_threads) {
    for (int i = 0; i < num_cpus; i++) {
        printf("CPU %d:\n", CPUsMachine[i].cpu_id);

        for (int j = 0; j < num_cores; j++) {
            printf("Core %d:\n", CPUsMachine[i].cores[j].core_id);

            for (int k = 0; k < num_threads; k++) {
                printf("Thread %d\n", CPUsMachine[i].cores[j].threads[k].tid);
            }
        }
    }
}

// Función para simular el Process Generator
void process_generator_thread() {
    printf("Process generator activado  \n");
    struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
    pcb->PID = randPID;
    randPID++;
    strcpy(pcb->state,"WAITING");
    pcb->remainingTime = 2;
    pcb->next = NULL;
    addPCB(&myQueue, pcb);
    printf("Nuevo pcb creado: %d \n", pcb->PID);

}

// Función para simular el Timer Process Generator
void *timer1(void *args) {
    int clk1 = 0;
    int f1 = *((int *)args);
    pthread_mutex_lock(&mutex);
    while (1) {
        done++;
        clk1++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
        if (clk1 >= f1){
            process_generator_thread();
            clk1 = 0;

        }
    }
}


// Scheduler 
void scheduler(){
    printf("Activado scheduler \n");

    struct PCB *restarAux = myQueue.first;
    while(restarAux != myQueue.last){
        if (strcmp(restarAux->state, "RUNNING") == 0){
            restarAux->remainingTime = restarAux->remainingTime - 1;
            printf("Proceso: %d, Tiempo restante: %d \n", restarAux->PID, restarAux->remainingTime);
        }
        restarAux = restarAux->next;
    }

    for (int i = 0; i < num_cpus; i++) {
        for (int j = 0; j < num_cores; j++) {
            for (int k = 0; k < num_threads; k++) {
                //Si el hilo esta libre se le asigna el pcb
                if(CPUsMachine[i].cores[j].threads[k].process == NULL){
                    printf("Hilo: %d, Proceso: NULL \n", CPUsMachine[i].cores[j].threads[k].tid);              
                    struct PCB *aux = myQueue.first;
                    struct PCB *salida = NULL;
                    while(aux != salida && strcmp(aux->state, "WAITING")!= 0){
                        aux=aux->next;
                        salida = myQueue.first;
                    }
                    printf("Estado: %s \n", aux->state);
                    if (aux != NULL && aux != salida){
                        strcpy(aux->state,"RUNNING");
                        CPUsMachine[i].cores[j].threads[k].process=aux;
                        printf("Proceso %i añadido a hilo %i \n",aux->PID, CPUsMachine[i].cores[j].threads[k].tid);
                    }
                }
                //Si el hilo esta ocupado se mira a ver si el proceso a acabado
                else{
                    printf("Hilo: %d, Proceso: %d \n", CPUsMachine[i].cores[j].threads[k].tid, CPUsMachine[i].cores[j].threads[k].process->PID);              
                    if (CPUsMachine[i].cores[j].threads[k].process->remainingTime < 1){
                        deletePCB(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
                        printf ("Elimina proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                        CPUsMachine[i].cores[j].threads[k].process = NULL;

                    }                    
                }  
            }
        }
    }
}

 // Función para simular el Timer Scheduler
void *timer2(void *args) {
    int clk2 = 0;
    int f2 = *((int *)args);
    pthread_mutex_lock(&mutex);
    while (1) {
        done++;
        clk2++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
        if (clk2 >= f2){
            scheduler();
            clk2 = 0;

        }
    }
}

// Clock
void *clock_thread() {

    while (1) {
        usleep(200000); 
        pthread_mutex_lock(&mutex);
        while (done < 2){
            pthread_cond_wait(&cond1, &mutex);
        }

        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}


void createPhysicalMemory(struct PhysicalMemory p){
    for(int i = 0; i < MEM_SIZE; i++)
        p.memory[i] = 0;
}

//MAIN

int main(int argc, char *argv[]) {

    if (argc != 6) {
        printf("Uso: %s <num_cpus> <num_cores> <num_threads> <clk_process_generator> <clk_scheduler>\n", argv[0]);
        return 1;
    }

    num_cpus = atoi(argv[1]);
    num_cores = atoi(argv[2]);
    num_threads = atoi(argv[3]);

    int frecuencia1 = atoi(argv[4]);
    int frecuencia2 = atoi(argv[5]);

    initializeProcessQueue(&myQueue);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);

    CPUsMachine = initializeMachine();

    pthread_t tid1, tid2, tid3;

    if (pthread_create(&tid1, NULL, clock_thread, NULL) != 0){
        perror("Errr al crear hilo tid1");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&tid2, NULL, timer1, &frecuencia1) != 0){
        perror("Errr al crear hilo tid2");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tid3, NULL, timer2, &frecuencia2) != 0){
        perror("Errr al crear hilo tid3");
        exit(EXIT_FAILURE);
    }

    //printMachineInfo(CPUsMachine, num_cpus, num_cores, num_threads);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    freeMachine(CPUsMachine, num_cpus, num_cores);
    printf("terminado");

    return 0;
}



    // struct PCB* pcb1 = (struct PCB*)malloc(sizeof(struct PCB));
    // pcb1->PID = 1;
    // pcb1->next = NULL;
    // addPCB(&myQueue, pcb1);

    // struct PCB* pcb2 = (struct PCB*)malloc(sizeof(struct PCB));
    // pcb2->PID = 2;
    // pcb2->next = NULL;
    // addPCB(&myQueue, pcb2);

    // struct PCB* current = myQueue.first;
    // while (current != NULL) {
    //     printf("PID: %d\n", current->PID);
    //     current = current->next;   
    // }

    // current = myQueue.first;
    // while (current != NULL) {
    //     struct PCB* temp = current;
    //     current = current->next;
    //     free(temp);
    // }