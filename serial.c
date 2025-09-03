#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "linkedlist.h"

#define MAX_VALUE 65536 // 2^16


double run_threads(struct list_node_s **list_head,
                   int m,
                   double mMember,
                   double mInsert,
                   double mDelete,
                   int thread_count,
                   int program_type)
{
    if (program_type != 0)
    {
        fprintf(stderr, "Only serial version (program_type=0) is implemented in this function.\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // random seed

    // Calculate exact counts
    int Mem = (int)(m * mMember);
    int Ins = (int)(m * mInsert);
    int Del = m - Mem - Ins;

    // Operation counters
    int insOps = 0;
    int memOps = 0;
    int delOps = 0;
    int totOps = 0;
    printf("Running in serial mode with %d operations\n", m);

    // Start timing
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // Perform m operations
    while (totOps < m)
    {
        int rand_value = rand() % MAX_VALUE;
        int op = rand() % 3;

        if (op == 0 && insOps < Ins)
        {
            // Insert operation
            Insert(rand_value, list_head);
            insOps++;
            totOps++;
        }
        else if (op == 1 && delOps < Del)
        {
            // Delete operation
            Delete(rand_value, list_head);
            delOps++;
            totOps++;
        }
        else if (memOps < Mem)
        {
            // Member operation
            Member(rand_value, *list_head);
            memOps++;
            totOps++;
        }
    }

    // Stop timing
    gettimeofday(&stop, NULL);
    unsigned long elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000 +
                                 stop.tv_usec - start.tv_usec;

    // Free the list
    destructor(*list_head); // free all nodes
    *list_head = NULL;

    return elapsed_time;
}
