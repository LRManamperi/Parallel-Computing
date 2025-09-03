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

    // Populate list with exactly n unique random values
    int count = 0;
    while (count < n)
    {
        int val = rand() % MAX_VALUE;
        if (Insert(val, &head))
        { // Only count successful inserts
            count++;
        }
    }

    unsigned long elapsed = run_threads(&head, m, mmem, mins, mdel, thread_count, program_type);

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

        double required_accuracy = 0.05 * avg_time; // 5% of mean
        int required_samples = (int)ceil(pow((1.96 * std_dev) / required_accuracy, 2));

        printf("Required samples for 95%% CI within 5%%: %d\n", required_samples);

        const char *program_type_str = NULL;

        if (program_type == 0)
            program_type_str = "Serial";
        else if (program_type == 1)
            program_type_str = "Mutex";
        else if (program_type == 2)
            program_type_str = "RWLock";
        else
            program_type_str = "Unknown";

        // Write to CSV
        fprintf(fp, "%s,%d,%.2f,%.2f,%lu,%lu,%.2f,%.2f,%d\n",
                program_type_str, case_num, avg_time, std_dev, min_time, max_time, avg_time - margin_error, avg_time + margin_error, thread_count);
    }
}

// Function to run multiple tests and calculate statistics
void run_performance_tests(int program_type, FILE *fp)
{
    const int num_runs = 30;
    unsigned long times[num_runs];
    int thread_counts[] = {1, 2, 4, 8};

    printf("\n=== PERFORMANCE TESTING ===\n");
    if (program_type == 0)
    {
        write_to_csv(fp, num_runs, times, 1, 0);
    }
    else if (program_type == 1)
    {

        for (int i = 0; i <= 3; i++)
        {
            write_to_csv(fp, num_runs, times, thread_counts[i], 1);
        }
    }
    else if (program_type == 2)
    {
        for (int i = 0; i <= 3; i++)
        {
            write_to_csv(fp, num_runs, times, thread_counts[i], 2);
        }
    }

    printf("\nResults saved to performance_results.csv files\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <program_type>\n", argv[0]);
        printf("0 = Serial, 1 = Mutex, 2 = RWLock\n");
        return 1;
    }

    int program_type = atoi(argv[1]);
    if (program_type < 0 || program_type > 2)
    {
        printf("Invalid program type. Must be 0, 1, or 2.\n");
        return 1;
    }

    FILE *fp = fopen("performance_results_all_test.csv", "a"); // append mode
    if (!fp)
    {
        printf("Error opening file\n");
        return 1;
    }

    if (ftell(fp) == 0)
    { // if file is empty, write header
        fprintf(fp, "ProgramType,Case,Average(us),StdDev(us),Min(us),Max(us),95%% CI Lower(us),95%% CI Upper(us),Thread Count\n");
    }

    run_performance_tests(program_type, fp);

    fclose(fp);
    return 0;
}
