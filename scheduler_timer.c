// scheduler_timer.c

#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

//Variables
int freeThreads;
int waitingProcess = 0;
int processActivated = 0;

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

void processGoLast(struct ProcessQueue* myQueue, struct PCB* pcb){
    if(myQueue->first != NULL && pcb != NULL){
        if (pcb->next != pcb){
            if (myQueue->first == pcb){
                myQueue->first = pcb->next;
            }else {
                struct PCB *aux = myQueue->first;
                while (aux->next != pcb){
                    aux = aux->next;
                }
                aux->next = pcb->next;
                myQueue->last->next = pcb;
                pcb->next = myQueue->first;
            }
            myQueue->last = pcb;
        }else{
            printf("El proceso es el ultimo, error");
        }
    }else{
        printf("Pasar proceso a la ultima posicion, error");
    }
}

// Scheduler 
void scheduler(){
    if (processActivated == 1){
        printf("Activado scheduler \n");
        printf("Numero de hilos libres: %d \n", freeThreads);
        printf("Numero de procesos esperando: %d \n", waitingProcess);
        for (int i = 0; i < num_cpus; i++) {
            for (int j = 0; j < num_cores; j++) {
                for (int k = 0; k < num_threads; k++) {
                    //Si el hilo esta libre se le asigna el pcb
                    if(CPUsMachine[i].cores[j].threads[k].process == NULL && waitingProcess > 0){
                        struct PCB *aux = myQueue.first;
                        struct PCB *salida = NULL;
                        while(aux != salida && strcmp(aux->state, "WAITING")!= 0){
                            aux=aux->next;
                            salida = myQueue.first;
                        }
                        if (aux != NULL && aux != salida){
                            strcpy(aux->state,"RUNNING");
                            CPUsMachine[i].cores[j].threads[k].process=aux;
                            freeThreads--;
                            waitingProcess--;
                            printf("Proceso %i añadido a hilo %i \n",aux->PID, CPUsMachine[i].cores[j].threads[k].tid);
                        }
                    }
                    //Si el hilo esta ocupado 
                    else if (CPUsMachine[i].cores[j].threads[k].process != NULL){
                        //Si el proceso ha acabado
                        if (CPUsMachine[i].cores[j].threads[k].process->remainingTime == 0){
                            deletePCB(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
                            printf ("Se elimina proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                            CPUsMachine[i].cores[j].threads[k].process = NULL;
                            freeThreads++;
                            k--;                         
                            

                        }
                        //Si el quantum ha terminado, no hay hilos disponibles y hay procesos esperando
                        else if (CPUsMachine[i].cores[j].threads[k].process->quantum <= 0 &&  waitingProcess > freeThreads){
                            processGoLast(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
                            printf ("Se manda a la ultima posicion el proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                            CPUsMachine[i].cores[j].threads[k].process->quantum = 5;
                            strcpy(CPUsMachine[i].cores[j].threads[k].process->state,"WAITING");
                            CPUsMachine[i].cores[j].threads[k].process = NULL;
                            freeThreads++;
                            waitingProcess++;
                            k--;                
                        }
                        
                    }  
                }
            }
        }

        struct PCB *restarAux = myQueue.first;
        if (restarAux != myQueue.last){
            while(restarAux != myQueue.last){
                if (strcmp(restarAux->state, "RUNNING") == 0){
                    restarAux->remainingTime = restarAux->remainingTime - 1;
                    restarAux->quantum = restarAux->quantum - 1;
                    printf("Proceso: %d, Tiempo restante: %d  Quantum: %d \n", restarAux->PID, restarAux->remainingTime, restarAux->quantum);
                }
                restarAux = restarAux->next;
            } 
        }
        if (restarAux != NULL && strcmp(restarAux->state, "RUNNING") == 0){
            restarAux->remainingTime = restarAux->remainingTime - 1;
            restarAux->quantum = restarAux->quantum - 1;
            printf("Proceso: %d, Tiempo restante: %d  Quantum: %d \n", restarAux->PID, restarAux->remainingTime, restarAux->quantum);
        }

     
    }
}

 // Timer para el Scheduler
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
