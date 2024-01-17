//process_generator.c

#include "define.h"

//Variables
int randPID = 0;
int physicalMemory[MEM_SIZE];
char filename[20] = "prom/prog000.elf";

//Funcion para añadir PCB a la cola
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

// // Función para procesar una línea de instrucción del programa
// void processInstructionLine(const char* line) {
// }

//Funcion para leer programa
void loadProgram(struct PCB* pcb, const char* filename) {
    char direction_text[8]; 
    char direction_data[8];
    char line[8];

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error al abrir el archivo %s \n", filename);
        return ;
    }

    int k = 0;
    while(fgets(line, sizeof(line), file) != NULL){
        if (k == 0){
            sscanf(line, ".text %s", direction_text);
            pcb->mm.code = atoi(direction_text);
        }
        if(k == 1){
            sscanf(line, ".data %s", direction_data);
            pcb->mm.data = atoi(direction_data);
        }
        k++;
    }
    fclose(file);
}

// Función para simular el Process Generator Loader
void process_generator_loader() {
    if (max_process > 0 || max_process == -1){
        struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
        pcb->PID = randPID;
        randPID++;
        strcpy(pcb->state,"WAITING");
        pcb->remainingTime = rand() % 10 + 1;
        pcb->quantum = 5;
        pcb->next = NULL;

        // Parte 3
        loadProgram(pcb, filename);
        sprintf(filename, "prom/prog%03d.elf", randPID);

        addPCB(&myQueue, pcb);
        printf("Nuevo pcb creado: %d \n", pcb->PID);
        waitingProcess++;
        if (max_process > 0) max_process--;
    }
    else if (max_process == 0){
        if (myQueue.first == NULL){
            printf("Se han ejecutado todos los procesos posibles ha crear \n");
            exit(0);
        }  
    }
}

// Timer para el Process Generator
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
            process_generator_loader();
            clk1 = 0;

        }
    }
}
