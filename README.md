Para compilar:
gcc -pthread main.c clock_thread.c scheduler_timer.c process_generator.c -o main

Para ejecutar:
<num_cpu><num_cores><num_thread><frecuencia_process_generator><frecuencia_scheduler><type_scheduler (0:FIFO, 1:SJF, 2:Robin Round)><(OPCIONAL) max_process>

Ejemplos de ejecucion de todos los schedulers sin <max_process>:
./main 1 2 3 1 2 0

./main 1 2 3 1 2 1

./main 1 2 3 1 2 2


Ejemplos de ejecucion de todos los schedulers con <max_process>:
./main 1 2 3 1 2 0 30

./main 1 2 3 1 2 1 30

./main 1 2 3 1 2 2 30