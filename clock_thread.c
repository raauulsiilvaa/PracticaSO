// clock_thread.c

#include <pthread.h>
#include <unistd.h>

extern pthread_mutex_t mutex;
extern pthread_cond_t cond1, cond2;
extern int done;

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
