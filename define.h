// clock_thread.h

#ifndef CLOCK_THREAD_H
#define CLOCK_THREAD_H

#include <pthread.h>

extern pthread_mutex_t mutex;
extern pthread_cond_t cond1, cond2;
extern int done;

void *clock_thread();

#endif  // CLOCK_THREAD_H
