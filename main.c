#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "linkedlist.h"
#include "run_threads.h"

#define MAX_VALUE 65536 // 2^16

int n = 1000;  // initial population size
int m = 10000; // number of operations

// Main test function
void get_case_ops(int case_num, double *mmem, double *mins, double *mdel)
{
    // Set operation fractions based on case
    switch (case_num)
    {
    case 1:
    {
        *mmem = 0.99;
        *mins = 0.005;
        *mdel = 0.005;
        break;
    }
    case 2:
    {
        *mmem = 0.9;
        *mins = 0.05;
        *mdel = 0.05;
        break;
    }
    case 3:
    {
        *mmem = 0.5;
        *mins = 0.25;
        *mdel = 0.25;
        break;
    }
    default:
    {
        *mmem = 0.99;
        *mins = 0.005;
        *mdel = 0.005;
        break;
    }
    }
}

unsigned long run_experiment(int case_num, int thread_count, int program_type)
{
    double mmem, mins, mdel;
    get_case_ops(case_num, &mmem, &mins, &mdel);

    struct list_node_s *head = NULL;

    // Populate list with n random values
    for (int i = 0; i < n; i++)
    {
        int val = rand() % MAX_VALUE;
        Insert(val, &head);
    }
    // printf(m, mmem, mins, mdel, thread_count, program_type);

    unsigned long elapsed = run_threads(&head, m, mmem, mins, mdel, thread_count, program_type);

    // Free list
    struct list_node_s *curr = head;
    while (curr != NULL)
    {
        struct list_node_s *tmp = curr;
        curr = curr->next;
        free(tmp);
    }

    return elapsed;
}

void write_to_csv(FILE *fp, int num_runs, unsigned long times[], int thread_count, int program_type)
{
    for (int case_num = 1; case_num <= 3; case_num++)
    {
        printf("\n--- Case %d ---\n", case_num);

        unsigned long total_time = 0, min_time = ULONG_MAX, max_time = 0;

        for (int run = 0; run < num_runs; run++)
        {
            printf("Run %d/%d: ", run + 1, num_runs);
            times[run] = run_experiment(case_num, thread_count, program_type);
            total_time += times[run];

            if (times[run] < min_time)
                min_time = times[run];
            if (times[run] > max_time)
                max_time = times[run];
        }

        double avg_time = (double)total_time / num_runs;

        double variance = 0;
        for (int i = 0; i < num_runs; i++)
            variance += (times[i] - avg_time) * (times[i] - avg_time);
        variance /= num_runs;
        double std_dev = sqrt(variance);
        double margin_error = 1.96 * std_dev / sqrt(num_runs);

        printf("\nCase %d Results:\n", case_num);
        printf("Average time: %.2f us, StdDev: %.2f us, Min: %lu us, Max: %lu us\n",
               avg_time, std_dev, min_time, max_time);
        printf("95%% CI: %.2f ± %.2f us\n", avg_time, margin_error);
        printf("Range: [%.2f, %.2f] us\n", avg_time - margin_error, avg_time + margin_error);

        // Write to CSV
        fprintf(fp, "%d,%.2f,%.2f,%lu,%lu,%.2f,%.2f,%d\n",
                case_num, avg_time, std_dev, min_time, max_time, avg_time - margin_error, avg_time + margin_error, thread_count);
    }
}

// Function to run multiple tests and calculate statistics
void run_performance_tests(int program_type)
{
    const int num_runs = 30;
    unsigned long times[num_runs];
    int thread_counts[] = {1, 2, 4, 8};

    // CSV header

    printf("\n=== PERFORMANCE TESTING ===\n");
    if (program_type == 0)
    {
        FILE *fp = fopen("performance_results_serial.csv", "w");
        if (!fp)
        {
            printf("Error opening CSV file for writing.\n");
            return;
        }
        printf("\n=== Serial Program ===\n");
        fprintf(fp, "\n=== Serial Program ===\n");

        fprintf(fp, "Case,Average(us),StdDev(us),Min(us),Max(us),95%% CI Lower(us),95%% CI Upper(us),Thread Count\n");

        write_to_csv(fp, num_runs, times, 1, 0);

        fclose(fp);
    }
    else if (program_type == 1)
    {
        FILE *fp = fopen("performance_results_mutex.csv", "w");
        if (!fp)
        {
            printf("Error opening CSV file for writing.\n");
            return;
        }
        printf("\n=== Parallel Program (One Mutex for Entire LinkedList)===\n");
        fprintf(fp, "\n=== Parallel Program (One Mutex for Entire LinkedList)===\n");

        fprintf(fp, "Case,Average(us),StdDev(us),Min(us),Max(us),95%% CI Lower(us),95%% CI Upper(us),Thread Count\n");
        for (int i = 0; i <= 3; i++)
        {
            write_to_csv(fp, num_runs, times, thread_counts[i], 1);
        }
        fclose(fp);
    }
    else if (program_type == 2)
    {
        FILE *fp = fopen("performance_results_rwlock.csv", "w");
        if (!fp)
        {
            printf("Error opening CSV file for writing.\n");
            return;
        }
        printf("\n=== Parallel Program (Read Write Locks for Entire LinkedList)===\n");
        fprintf(fp, "\n=== Parallel Program (Read Write Locks for Entire LinkedList)===\n");

        fprintf(fp, "Case,Average(us),StdDev(us),Min(us),Max(us),95%% CI Lower(us),95%% CI Upper(us),Thread Count\n");
        for (int i = 0; i <= 3; i++)
        {
            write_to_csv(fp, num_runs, times, thread_counts[i], 1);
        }
        fclose(fp);
    }

    printf("\nResults saved to performance_results.csv files\n");
}

int main()
{
    srand(time(NULL)); // random seed
    run_performance_tests(1);

    return 0;
}
