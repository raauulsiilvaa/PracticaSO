//main.c

#include "define.h" 

//Variables necesarias

struct ProcessQueue myQueue;

int num_cpus;    
int num_cores;   
int num_threads; 

int type_scheduler = 5;
int max_process;


struct CPU* CPUsMachine; 

//Funcion que inicializa la lista de pcb (procesos)
void initializeProcessQueue(struct ProcessQueue* myQueue){
    myQueue->first = NULL;
    myQueue->last = NULL;
}

//Funcion para inicializar la memoria fisica
void initializePhysicalMemory(){
    for (int i = 0; i > MEM_SIZE; i++){
        physicalMemory[i] = 0;
    }
}

//Funcion que inicializa la maquina con los cpus, cores y hilos
struct CPU* initializeMachine() {
    // Asignar memoria para un array de CPUs
    CPUsMachine = (struct CPU*)malloc(num_cpus * sizeof(struct CPU));

    freeThreads =  num_cpus * num_cores * num_threads;

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

//MAIN

int main(int argc, char *argv[]) {

    if (argc < 7 || argc > 8) {
        printf("Uso: %s <num_cpus> <num_cores> <num_threads> <clk_process_generator> <clk_scheduler> <type_scheduler (0:FIFO, 1:SJF, 2:Robin Round)> <(OPCIONAL) max_process>\n", argv[0]);
        return 1;
    }   
    if (argc == 7){
        if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || atoi(argv[4]) <= 0 || atoi(argv[5]) <= 0 || (atoi(argv[6]) != 0 && atoi(argv[6]) != 1 && atoi(argv[6]) != 2)) {
        printf("Todos los argumentos deben ser números positivos mayores que 0, menos el type_scheduler(6) que debe ser 0, 1 o 2 \n");
        return 2;
        }
        max_process = -1;
    }
    if (argc == 8){
        if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || atoi(argv[4]) <= 0 || atoi(argv[5]) <= 0 || (atoi(argv[6]) != 0 && atoi(argv[6]) != 1 && atoi(argv[6]) != 2) || atoi(argv[7]) <= 0) {
        printf("Todos los argumentos deben ser números positivos mayores que 0, menos el type_scheduler(6) que debe ser 0, 1 o 2\n");
        return 3;
        }
        max_process = atoi(argv[7]);
    }
    
    
    num_cpus = atoi(argv[1]);
    num_cores = atoi(argv[2]);
    num_threads = atoi(argv[3]);

    int frecuencia1 = atoi(argv[4]);
    int frecuencia2 = atoi(argv[5]);

    type_scheduler = atoi(argv[6]);
 
    initializeProcessQueue(&myQueue);
    // initializePhysicalMemory();

    CPUsMachine = initializeMachine(); 

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);

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

    printf("Final");

    return 0;
}
