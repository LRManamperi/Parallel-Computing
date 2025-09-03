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

// double get_time()
// {
//     struct timeval t;
//     gettimeofday(&t, NULL);
//     return t.tv_sec + t.tv_usec / 1e6;
// }

// void *thread_worker(void *args)
// {
//     thread_params_t *params = (thread_params_t *)args;
//     int ops_per_thread = params->m / params->thread_count;

//     for (int i = 0; i < ops_per_thread; i++)
//     {
//         double prob = (double)rand() / RAND_MAX;
//         int val = rand() % 65536;

//         pthread_mutex_lock(params->mutex);

//         if (prob < params->mMember)
//         {
//             Member(val, *(params->head));
//         }
//         else if (prob < params->mMember + params->mInsert)
//         {
//             Insert(val, params->head);
//         }
//         else
//         {
//             Delete(val, params->head);
//         }

//         pthread_mutex_unlock(params->mutex);
//     }
//     return NULL;
// }

void *thread_worker(void *args)
{
    thread_params_t *params = (thread_params_t *)args;

    int totOps = 0;
    int memOps = 0, insOps = 0, delOps = 0;

    // Calculate how many of each operation this thread should perform
    int Mem = (int)(params->mMember * params->m / params->thread_count);
    int Ins = (int)(params->mInsert * params->m / params->thread_count);
    int Del = (int)(params->mDelete * params->m / params->thread_count);

    while (totOps < Mem + Ins + Del)
    {
        int rand_value = rand() % 65536;
        int op = rand() % 3;

        if (op == 0 && insOps < Ins)
        {
            pthread_mutex_lock(params->mutex);
            Insert(rand_value, params->head);
            pthread_mutex_unlock(params->mutex);
            insOps++;
            totOps++;
        }
        else if (op == 1 && delOps < Del)
        {
            pthread_mutex_lock(params->mutex);
            Delete(rand_value, params->head);
            pthread_mutex_unlock(params->mutex);
            delOps++;
            totOps++;
        }
        else if (op == 2 && memOps < Mem)
        {
            pthread_mutex_lock(params->mutex);
            Member(rand_value, *(params->head));
            pthread_mutex_unlock(params->mutex);
            memOps++;
            totOps++;
        }
    }

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
