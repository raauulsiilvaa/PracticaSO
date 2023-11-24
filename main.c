#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


//Estrcutura PCBs
struct PCB {
    int PID;
    struct PCB* siguiente;
};

struct ProcessQueue {
    struct PCB* first;
    struct PCB* last;
};

void initializeProcessQueue(struct ProcessQueue* lista){
    lista->first = NULL;
    lista->last = NULL;
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

//Estructura CPU, Core e Hilos
struct Thread {
    int tid;
};

struct Core {
    int core_id;        
    struct Thread* threads;    
};

struct CPU {
    int cpu_id;    
    struct Core* cores;   
};

struct Machine {
    int num_cpus;    
    int num_cores;   
    int num_threads; 
    struct CPU* cpus;      
};

void initializeMachine(struct Machine* machine) {
    machine->cpus = (struct CPU*)malloc(machine->num_cpus * sizeof(struct CPU));

    for (int i = 0; i < machine->num_cpus; i++) {
        machine->cpus[i].cpu_id = i + 1;
        machine->cpus[i].cores = (struct Core*)malloc(machine->num_cores * sizeof(struct Core));

        for (int j = 0; j < machine->num_cores; j++) {
            machine->cpus[i].cores[j].core_id = j + 1;
            machine->cpus[i].cores[j].threads = (struct Thread*)malloc(machine->num_threads * sizeof(struct Thread));

            for (int k = 0; k < machine->num_threads; k++) {
                machine->cpus[i].cores[j].threads[k].tid = k + 1;
            }
        }
    }
}

// Función para liberar la memoria asignada para la máquina
void freeMachine(struct Machine* machine) {
    for (int i = 0; i < machine->num_cpus; i++) {
        for (int j = 0; j < machine->num_cores; j++) {
            free(machine->cpus[i].cores[j].threads);
        }
        free(machine->cpus[i].cores);
    }
    free(machine->cpus);
}

// Función para imprimir la información de la máquina
void printMachineInfo(struct Machine* machine) {
    for (int i = 0; i < machine->num_cpus; i++) {
        printf("CPU %d:\n", machine->cpus[i].cpu_id);

        for (int j = 0; j < machine->num_cores; j++) {
            printf("  Core %d:\n", machine->cpus[i].cores[j].core_id);

            for (int k = 0; k < machine->num_threads; k++) {
                printf("    Thread %d\n", machine->cpus[i].cores[j].threads[k].tid);
            }
        }
    }
}

// Función para simular el Process Generator
void process_generator_thread() {
    printf("Process generator activado");
}

pthread_mutex_t mutex;
pthread_cond_t cond1, cond2;
int done = 0;
int clk1 = 0;
int clk2 = 0;

// Estructura clock, timer y process generator
void *clock_thread() {

    while (1) {
        usleep(100000); 
        pthread_mutex_lock(&mutex);
        while (done < 2){
            pthread_cond_wait(&cond1, &mutex);
        }

        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

// Función para simular el Timer Process Generator
void *timer1() {
    pthread_mutex_lock(&mutex);
    while (1) {
        done++;
        clk1++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
        if (clk1 == 3){
            printf("Process queue activado \n");
            clk1 = 0;

        }
    }

}

// Función para simular el Timer Scheduler
void *timer2() {
    pthread_mutex_lock(&mutex);
    while (1) {
        done++;
        clk2++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
        if (clk2 == 5){
            printf("Activado scheduler \n");
            clk2 = 0;

        }
    }
}


//MAIN
int main(int argc, char *argv[]) {

    // if (argc != 4) {
    //     printf("Uso: %s <num_cpus> <num_cores> <num_threads>\n", argv[0]);
    //     return 1;
    // }

    // struct Machine myMachine;
    // myMachine.num_cpus = atoi(argv[1]);
    // myMachine.num_cores = atoi(argv[2]);
    // myMachine.num_threads = atoi(argv[3]);

    // initializeMachine(&myMachine);
    // printMachineInfo(&myMachine);
    // freeMachine(&myMachine);

    // struct ProcessQueue myQueue;
    // initializeProcessQueue(&myQueue);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);


    pthread_t tid1, tid2, tid3;
    if (pthread_create(&tid1, NULL, clock_thread, NULL) != 0){
        perror("Errr al crear hilo");
        exit(EXIT_FAILURE);
    }
    if (    pthread_create(&tid2, NULL, timer1, NULL) != 0){
        perror("Errr al crear hilo");
        exit(EXIT_FAILURE);
    }
    pthread_create(&tid3, NULL, timer2, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);


    // struct PCB* pcb1 = (struct PCB*)malloc(sizeof(struct PCB));
    // pcb1->PID = 1;
    // pcb1->siguiente = NULL;
    // addPCB(&myQueue, pcb1);

    // struct PCB* pcb2 = (struct PCB*)malloc(sizeof(struct PCB));
    // pcb2->PID = 2;
    // pcb2->siguiente = NULL;
    // addPCB(&myQueue, pcb2);

    // struct PCB* current = myQueue.first;
    // while (current != NULL) {
    //     printf("PID: %d\n", current->PID);
    //     current = current->siguiente;   
    // }

    // current = myQueue.first;
    // while (current != NULL) {
    //     struct PCB* temp = current;
    //     current = current->siguiente;
    //     free(temp);
    // }

    return 0;



    // // Crear hilos para cada subsistema
    // pthread_t clock_tid, timer_tid, process_generator_tid;
    // pthread_create(&clock_tid, NULL, clock_thread, NULL);
    // pthread_create(&timer_tid, NULL, timer_thread, NULL);
    // pthread_create(&process_generator_tid, NULL, process_generator_thread, NULL);

    // // Esperar a que los hilos terminen (esto no debería suceder en un kernel real)
    // pthread_join(clock_tid, NULL);
    // pthread_join(timer_tid, NULL);
    // pthread_join(process_generator_tid, NULL);

}
