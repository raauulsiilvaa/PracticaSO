Para compilar:
gcc pthread.h main.c clock_thread.c scheduler_timer.c process_generator.c -o main

Para ejecutar:
<num_cpu><num_cores><num_thread><frecuencia process_generator><frecuencia scheduler>

(Por ejemplo):
./main 1 2 3 1 2
