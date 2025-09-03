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

//         if (prob < params->mMember)
//         {
//             // Read lock for Member
//             pthread_rwlock_rdlock(params->rwlock);
//             Member(val, *(params->head));
//             pthread_rwlock_unlock(params->rwlock);
//         }
//         else if (prob < params->mMember + params->mInsert)
//         {
//             // Write lock for Insert
//             pthread_rwlock_wrlock(params->rwlock);
//             Insert(val, params->head);
//             pthread_rwlock_unlock(params->rwlock);
//         }
//         else
//         {
//             // Write lock for Delete
//             pthread_rwlock_wrlock(params->rwlock);
//             Delete(val, params->head);
//             pthread_rwlock_unlock(params->rwlock);
//         }
//     }
//     return NULL;
// }

void *thread_worker(void *args)
{
    thread_params_t *params = (thread_params_t *)args;

    int totOps = 0;
    int memOps = 0, insOps = 0, delOps = 0;

    int Mem = (int)(params->mMember * params->m / params->thread_count);
    int Ins = (int)(params->mInsert * params->m / params->thread_count);
    int Del = (int)(params->mDelete * params->m / params->thread_count);

    while (totOps < Mem + Ins + Del)
    {
        int rand_value = rand() % 65536;
        int op = rand() % 3;

        if (op == 0 && insOps < Ins)
        {
            // Exclusive lock for Insert
            pthread_rwlock_wrlock(params->rwlock);
            Insert(rand_value, params->head);
            pthread_rwlock_unlock(params->rwlock);
            insOps++;
            totOps++;
        }
        else if (op == 1 && delOps < Del)
        {
            // Exclusive lock for Delete
            pthread_rwlock_wrlock(params->rwlock);
            Delete(rand_value, params->head);
            pthread_rwlock_unlock(params->rwlock);
            delOps++;
            totOps++;
        }
        else if (op == 2 && memOps < Mem)
        {
            // Shared lock for Member
            pthread_rwlock_rdlock(params->rwlock);
            Member(rand_value, *(params->head));
            pthread_rwlock_unlock(params->rwlock);
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
