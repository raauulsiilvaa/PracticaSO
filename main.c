#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>


//Estrucuturas Necesarias

struct PCB {
    int PID;
    char state[15];
    int priority;
    struct PCB* next;
};

struct ProcessQueue {
    struct PCB* first;
    struct PCB* last;
};

struct Thread {
    int tid;
    struct PCB* process;
};

struct Core {
    int core_id;        
    struct Thread* threads;    
};

struct CPU {
    int cpu_id;    
    struct Core* cores;   
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
    pcb->priority = 1;
    pcb->next = NULL;
    addPCB(&myQueue, pcb);
    printf("Nuevo pcb creado: %d \n", pcb->PID);
    printf("Pcb añadido last: %d \n", myQueue.last->PID);
    printf("Pcb añadido first: %d \n", myQueue.first->PID);

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

// Cambio de contexto
void cambio_contexto() {
    int id, min = 1000;

    // Recorrer los hilos
    for (int i = 0; i < num_cpus; i++) {
        for (int j = 0; j < num_cores; j++) {
            for (int k = 0; k < num_threads; k++) {

                
                
                // Si el hilo está ocupado y tiene menor prioridad
                //if (CPUsMachine[i].cores[j].threads[k].process != NULL &&
                //    CPUsMachine[i].cores[j].threads[k].process->priority < min) {
                //  min = CPUsMachine[i].cores[j].threads[k].process->priority;
                //    id = CPUsMachine[i].cores[j].threads[k].process->PID;
                }
            }
        }
    }

    // Realizar acciones con el hilo seleccionado
    // Por ejemplo, imprimir el proceso con menor prioridad
    printf("Cambio de contexto: Proceso con PID %d \n", id);
}

// Scheduler round robin
void scheduler(){
    printf("Activado scheduler \n");
    // struct Core* current_core;
    // struct Thread* current_thread;

    for (int i = 0; i < num_cpus; i++) {
        for (int j = 0; j < num_cores; j++) {
            for (int k = 0; k < num_threads; k++) {
                printf("hola");
                //Si el hilo esta libre se le asigna el pcb
                // printf("El id de el thread es: %i \n", CPUsMachine[i].cores[j].threads[k].tid);
                
                if(CPUsMachine[i].cores[j].threads[k].process == NULL){
                    printf("Hilo libre\n");
                    struct PCB *aux = myQueue.first;
                    struct PCB *salida = NULL;
                    while(aux != salida && strcmp(aux->state, "WAITING")!= 0){
                        printf("Estado: %s", aux->state);
                        aux=aux->next;
                        salida = myQueue.first;
                    }
                    if (aux != NULL && aux != salida){
                        strcpy(aux->state,"RUNNING");
                        CPUsMachine[i].cores[j].threads[k].process=aux;
                        printf("Proceso %i añadido a hilo %i",aux->PID, CPUsMachine[i].cores[j].threads[k].tid);
                    }
                }
                else{
                    printf("Cambio de contexto\n");
                    
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