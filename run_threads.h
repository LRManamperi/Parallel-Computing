// threads.h
#ifndef RUN_THREADS_H
#define RUN_THREADS_H

#include "linkedlist.h"

double run_threads(struct list_node_s **list_head,
                   int m,
                   double mMember,
                   double mInsert,
                   double mDelete,
                   int thread_count,
                   int program_type);

#endif
