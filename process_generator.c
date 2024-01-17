//process_generator.c


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "define.h"


//Variables
int randPID = 0;
int physicalMemory[MEM_SIZE];

void initializePhysicalMemory(){
    for (int i = 0; i > MEM_SIZE; i++){
        physicalMemory[i] = 0;
    }
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


// Función para simular el Process Generator
void process_generator_thread() {
    printf("Process generator activado  \n");
    waitingProcess++;
    processActivated = 1;

    //initializePhysicalMemory();

    struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
    pcb->PID = randPID;
    randPID++;
    strcpy(pcb->state,"WAITING");
    pcb->remainingTime = rand() % 10 + 1;
    pcb->quantum = 5;
    pcb->next = NULL;
    addPCB(&myQueue, pcb);
    printf("Nuevo pcb creado: %d \n", pcb->PID);
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
            process_generator_thread();
            clk1 = 0;

        }
    }
}
