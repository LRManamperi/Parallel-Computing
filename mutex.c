#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "linkedlist.h"

typedef struct
{
    struct list_node_s **head;
    int m;
    double mMember, mInsert, mDelete;
    int thread_count;
    pthread_mutex_t *mutex;
} thread_params_t;

void *thread_worker(void *args)
{
    thread_params_t *params = (thread_params_t *)args;

    // Thread-local RNG seed
    unsigned int seed = time(NULL) ^ (unsigned long)pthread_self();

    // Calculate how many of each operation this thread should perform
    int Mem = (int)(params->mMember * params->m / params->thread_count);
    int Ins = (int)(params->mInsert * params->m / params->thread_count);
    int Del = (int)(params->mDelete * params->m / params->thread_count);

    int totalOps = Mem + Ins + Del;

    // Step 1: Create an array of operations
    int *ops = malloc(totalOps * sizeof(int));
    int idx = 0;
    for (int i = 0; i < Mem; i++) ops[idx++] = 2;  // Member
    for (int i = 0; i < Ins; i++) ops[idx++] = 0;  // Insert
    for (int i = 0; i < Del; i++) ops[idx++] = 1;  // Delete

    // Step 2: Shuffle the array (Fisher-Yates)
    for (int i = totalOps - 1; i > 0; i--)
    {
        int j = rand_r(&seed) % (i + 1);
        int tmp = ops[i];
        ops[i] = ops[j];
        ops[j] = tmp;
    }

    // Step 3: Execute operations
    for (int i = 0; i < totalOps; i++)
    {
        int val = rand_r(&seed) % 65536;

        pthread_mutex_lock(params->mutex);

        if (ops[i] == 0)
            Insert(val, params->head);
        else if (ops[i] == 1)
            Delete(val, params->head);
        else
            Member(val, *(params->head));

        pthread_mutex_unlock(params->mutex);
    }

    free(ops);
    return NULL;
}

double run_threads(struct list_node_s **list_head,
                   int m,
                   double mMember,
                   double mInsert,
                   double mDelete,
                   int thread_count,
                   int program_type)
{
    if (program_type != 1)
    {
        fprintf(stderr, "Only mutex version (program_type=1) is implemented in this function.\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL)); // random seed
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    thread_params_t params = {list_head, m, mMember, mInsert, mDelete,
                              thread_count, &mutex};


    printf("Running in mutex mode with %d threads and %d operations\n", thread_count, m);

    // Start timing
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    for (long i = 0; i < thread_count; i++)
        pthread_create(&threads[i], NULL, thread_worker, &params);

    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    // Stop timing
    gettimeofday(&stop, NULL);
    unsigned long elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000 +
                                 stop.tv_usec - start.tv_usec;

    // Free the list
    destructor(*list_head); // free all nodes
    *list_head = NULL;

    pthread_mutex_destroy(&mutex);
    free(threads);

    return elapsed_time;
}
