// clock_thread.c

#include "define.h"
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond1, cond2;

int done = 0;

void *clock_thread() {
    while (1) {
        usleep(200000);
        pthread_mutex_lock(&mutex);
        while (done < 2) {
            pthread_cond_wait(&cond1, &mutex);
        }

        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}
