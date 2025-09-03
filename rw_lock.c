#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <pthread.h>
#include "linkedlist.h"

typedef struct
{
    struct list_node_s **head;
    int m;
    double mMember, mInsert, mDelete;
    int thread_count;
    pthread_rwlock_t *rwlock;
} thread_params_t;

void *thread_worker(void *args)
{
    thread_params_t *params = (thread_params_t *)args;

    // Thread-local RNG seed
    unsigned int seed = time(NULL) ^ (unsigned long)pthread_self();

    // Calculate exact number of operations per thread
    int Mem = (int)(params->mMember * params->m / params->thread_count);
    int Ins = (int)(params->mInsert * params->m / params->thread_count);
    int Del = (int)(params->mDelete * params->m / params->thread_count);
    int totalOps = Mem + Ins + Del;

    // Step 1: Create an array with exact operations
    int *ops = malloc(totalOps * sizeof(int));
    int idx = 0;
    for (int i = 0; i < Mem; i++) ops[idx++] = 2;  // Member
    for (int i = 0; i < Ins; i++) ops[idx++] = 0;  // Insert
    for (int i = 0; i < Del; i++) ops[idx++] = 1;  // Delete

    // Step 2: Shuffle operations array (Fisher-Yates)
    for (int i = totalOps - 1; i > 0; i--)
    {
        int j = rand_r(&seed) % (i + 1);
        int tmp = ops[i];
        ops[i] = ops[j];
        ops[j] = tmp;
    }

    // Step 3: Execute operations with correct locks
    for (int i = 0; i < totalOps; i++)
    {
        int val = rand_r(&seed) % 65536;

        if (ops[i] == 0)
        {
            // Insert (write lock)
            pthread_rwlock_wrlock(params->rwlock);
            Insert(val, params->head);
            pthread_rwlock_unlock(params->rwlock);
        }
        else if (ops[i] == 1)
        {
            // Delete (write lock)
            pthread_rwlock_wrlock(params->rwlock);
            Delete(val, params->head);
            pthread_rwlock_unlock(params->rwlock);
        }
        else
        {
            // Member (read lock)
            pthread_rwlock_rdlock(params->rwlock);
            Member(val, *(params->head));
            pthread_rwlock_unlock(params->rwlock);
        }
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
    if (program_type != 2) // 2 = read-write lock version
    {
        fprintf(stderr, "Only read-write lock version (program_type=2) is implemented in this function.\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL)); // random seed

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    pthread_rwlock_t rwlock;
    pthread_rwlock_init(&rwlock, NULL);

    thread_params_t params = {list_head, m, mMember, mInsert, mDelete,
                              thread_count, &rwlock};

    // double start = get_time();

    printf("Running in read-write lock mode with %d threads and %d operations\n", thread_count, m);
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
    pthread_rwlock_destroy(&rwlock);
    free(threads);

    return elapsed_time;
}
