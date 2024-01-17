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

// //Funcion para convertir las direccion de los programas (hexadecimal) en las de physicalMemory(decimal)
// int turnInMemory(char direccion[20]) {
//     int decimal = 0;
//     int length = strlen(direccion);

//     for (int i = 0; i < length; i++){
//         char currentChar = direccion[i];

//         int digitValue;
//         if (currentChar >= '0' && currentChar <= '9') digitValue = currentChar - '0';
//         else if(currentChar >= 'A' && currentChar <= 'F') digitValue = 10 + (currentChar - 'A');
//         else if(currentChar >= 'a' && currentChar <= 'f') digitValue = 10 + (currentChar - 'a');

//         decimal += digitValue * pow(16, length - i - 1);
//     }

//     if(decimal == 0) return 0;
//     else{
//         return decimal / 4 - 1;
//     }
// }

// //Funcion para leer los programas
// void loadProgram(struct PCB* pcb, const char* filename) {
//     char direction_text[8]; 
//     char direction_data[8];
//     char line[8];

//     FILE* file = fopen(filename, "r");

//     if (file == NULL) {
//         printf("Error al abrir el archivo %s \n", filename);
//         return ;
//     }

//     int k = 0;
//     while(fgets(line, sizeof(line), file) != NULL){
//         if (k == 0){
//             sscanf(line, ".text %s", direction_text);
//             pcb->mm.code = turnInMemory(direction_text);
//         }
//         if(k == 1){
//             sscanf(line, ".data %s", direction_data);
//             pcb->mm.data = turnInMemory(direction_data);
//         }
//         physicalMemory[pcb->mm.pgb+k] = atoi(line);
//         k++;

//     }
//     printf("%s .text: %i, .data: %i \n", filename, pcb->mm.code, pcb->mm.data+pcb->mm.pgb);
//     fclose(file);
// }

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
        // loadProgram(pcb, filename);
        // sprintf(filename, "prom/prog%03d.elf", randPID);

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
