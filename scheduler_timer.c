// scheduler_timer.c

#include "define.h"

//Variables
int freeThreads;
int waitingProcess = 0;

//Funcion para borrar un pcb de la lista
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
        free(pcb);
    }
    else{
        printf("Delete incorrecto");
    }

}

//Funcion para mandar un pcb a la ultima posicion de la lista
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

//Scheduler tipo Robin Round
void scheduler_RobinRound(){
    if (myQueue.first != NULL){
        printf("Activado scheduler Robin Round\n");
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
                            printf ("Se elimina proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                            deletePCB(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
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
        //Resta el remainingTime y quatum a los procesos que se estan ejecutando
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

// Scheduler tipo FIFO
void scheduler_FIFO(){
    if (myQueue.first != NULL){
        printf("Activado scheduler FIFO \n");
        printf("Numero de hilos libres: %d \n", freeThreads);
        printf("Numero de procesos esperando: %d \n", waitingProcess);
        for (int i = 0; i < num_cpus; i++) {
            for (int j = 0; j < num_cores; j++) {
                for (int k = 0; k < num_threads; k++) {
                    //Si el hilo esta libre se le asigna el primer pcb
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
                            printf ("Se elimina proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                            deletePCB(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
                            CPUsMachine[i].cores[j].threads[k].process = NULL;
                            freeThreads++;
                            k--;                         
                            
                        }
                    }  
                }
            }
        }
        //Resta el remainingTime a los procesos que se estan ejecutando
        struct PCB *restarAux = myQueue.first;
        if (restarAux != myQueue.last){
            while(restarAux != myQueue.last){
                if (strcmp(restarAux->state, "RUNNING") == 0){
                    restarAux->remainingTime = restarAux->remainingTime - 1;
                    printf("Proceso: %d, Tiempo restante: %d  \n", restarAux->PID, restarAux->remainingTime);
                }
                restarAux = restarAux->next;
            } 
        }
        if (restarAux != NULL && strcmp(restarAux->state, "RUNNING") == 0){
            restarAux->remainingTime = restarAux->remainingTime - 1;
            restarAux->quantum = restarAux->quantum - 1;
            printf("Proceso: %d, Tiempo restante: %d \n", restarAux->PID, restarAux->remainingTime);
        }

     
    }
}

// Scheduler tipo SJF
void scheduler_SJF(){
    if (myQueue.first != NULL){
        printf("Activado scheduler SJF \n");
        printf("Numero de hilos libres: %d \n", freeThreads);
        printf("Numero de procesos esperando: %d \n", waitingProcess);
        for (int i = 0; i < num_cpus; i++) {
            for (int j = 0; j < num_cores; j++) {
                for (int k = 0; k < num_threads; k++) {
                    //Si el hilo esta libre se busca el pcb con menos remainingTime
                    if(CPUsMachine[i].cores[j].threads[k].process == NULL && waitingProcess > 0){
                        int lowerTime = 1000;
                        struct PCB *aux = myQueue.first;
                        struct PCB *salida = NULL;
                        struct PCB *lowerAux = NULL;
                        while(aux != salida){
                            if (strcmp(aux->state, "WAITING")== 0 && aux->remainingTime < lowerTime){
                                lowerTime = aux->remainingTime;
                                lowerAux = aux;
                            }
                            aux=aux->next;
                            salida = myQueue.first;
                        }
                        if (lowerAux != NULL){
                            strcpy(lowerAux->state,"RUNNING");
                            CPUsMachine[i].cores[j].threads[k].process=lowerAux;
                            freeThreads--;
                            waitingProcess--;
                            printf("Proceso: %d añadido a hilo: %i con el menor tiempo: %d \n", lowerAux->PID, CPUsMachine[i].cores[j].threads[k].tid, lowerTime);
                        }
                    }
                    //Si el hilo esta ocupado 
                    else if (CPUsMachine[i].cores[j].threads[k].process != NULL){
                        //Si el proceso ha acabado
                        if (CPUsMachine[i].cores[j].threads[k].process->remainingTime == 0){
                            printf ("Se elimina proceso: %d \n", CPUsMachine[i].cores[j].threads[k].process->PID);
                            deletePCB(&myQueue, CPUsMachine[i].cores[j].threads[k].process);
                            CPUsMachine[i].cores[j].threads[k].process = NULL;
                            freeThreads++;
                            k--;                         
                            
                        }
                    }  
                }
            }
        }
        //Resta el remainingTime a los procesos que se estan ejecutando
        struct PCB *restarAux = myQueue.first;
        if (restarAux != myQueue.last){
            while(restarAux != myQueue.last){
                if (strcmp(restarAux->state, "RUNNING") == 0){
                    restarAux->remainingTime = restarAux->remainingTime - 1;
                    printf("Proceso: %d, Tiempo restante: %d  \n", restarAux->PID, restarAux->remainingTime);
                }
                restarAux = restarAux->next;
            } 
        }
        if (restarAux != NULL && strcmp(restarAux->state, "RUNNING") == 0){
            restarAux->remainingTime = restarAux->remainingTime - 1;
            restarAux->quantum = restarAux->quantum - 1;
            printf("Proceso: %d, Tiempo restante: %d \n", restarAux->PID, restarAux->remainingTime);
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
            if (type_scheduler == 0){
                scheduler_FIFO();
            }else if(type_scheduler == 1){
                scheduler_SJF();
            }else if(type_scheduler == 2){
                scheduler_RobinRound();
            }
            clk2 = 0;

        }
    }
}
