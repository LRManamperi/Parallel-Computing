//
// Created by DELL on 9/3/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>


#define MAX 65536  // 2^16

// Node structure for linked list
typedef struct node {
    int data;
    struct node* next;
} node;

// Function prototypes
short Insert(int value, node** head);
short Delete(int value, node** head);
short Member(int value, node* head);
void destructor(node* head);
unsigned long test_serial_run(int case_num);
bool is_unique(int value, node* head);

// Insert function - returns 1 if successful, 0 if value already exists
short Insert(int value, node** head) {
    // Check if value already exists
    if (Member(value, *head)) {
        return 0; // Value already exists
    }

    node* new_node = (node*)malloc(sizeof(node));
    if (new_node == NULL) {
        return 0; // Memory allocation failed
    }

    new_node->data = value;
    new_node->next = *head;
    *head = new_node;
    return 1; // Success
}

// Delete function - returns 1 if successful, 0 if value not found
short Delete(int value, node** head) {
    if (*head == NULL) {
        return 0; // List is empty
    }

    node* current = *head;

    // If head node contains the value to delete
    if (current->data == value) {
        *head = current->next;
        free(current);
        return 1;
    }

    // Search for the node to delete
    while (current->next != NULL && current->next->data != value) {
        current = current->next;
    }

    // If value not found
    if (current->next == NULL) {
        return 0;
    }

    // Delete the node
    node* node_to_delete = current->next;
    current->next = node_to_delete->next;
    free(node_to_delete);
    return 1;
}

// Member function - returns 1 if value exists, 0 otherwise
short Member(int value, node* head) {
    node* current = head;
    while (current != NULL) {
        if (current->data == value) {
            return 1; // Found
        }
        current = current->next;
    }
    return 0; // Not found
}

// Helper function to check if a value is unique (not in list)
bool is_unique(int value, node* head) {
    return !Member(value, head);
}

// Destructor to free all nodes
void destructor(node* head) {
    node* current = head;
    while (current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
}

// Main test function
unsigned long test_serial_run(int case_num) {
    node* head = NULL;
    float mmem, mins, mdel;

    int m = 10000; // Number of operations

    // Set operation fractions based on case
    switch (case_num) {
        case 1: {
            mmem = 0.99;
            mins = 0.005;
            mdel = 0.005;
            break;
        }
        case 2: {
            mmem = 0.9;
            mins = 0.05;
            mdel = 0.05;
            break;
        }
        case 3: {
            mmem = 0.5;
            mins = 0.25;
            mdel = 0.25;
            break;
        }
        default: {
            mmem = 0.99;
            mins = 0.005;
            mdel = 0.005;
            break;
        }
    }

    // Calculate number of each operation type
    int Mem = (int)(m * mmem);
    int Ins = (int)(m * mins);
    int Del = (int)(m * mdel);

    // Operation counters
    int insOps = 0;
    int memOps = 0;
    int delOps = 0;
    int totOps = 0;

    int n = 1000; // Number of initial elements
    int count = 0;

    // Set different random seed for each execution
    srand(time(NULL));

    // Generate initial linked list with n unique random numbers
    while (count < n) {
        int rand_value = rand() % MAX;
        if (is_unique(rand_value, head)) {
            Insert(rand_value, &head);
            count++;
        }
    }

    printf("Initial list populated with %d unique values\n", n);
    printf("Starting %d operations: %d Member, %d Insert, %d Delete\n",
           m, Mem, Ins, Del);

    // Start timing
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // Perform m operations
    while (totOps < m) {
        int rand_value = rand() % MAX;
        int op = rand() % 3;

        if (op == 0 && insOps < Ins) {
            // Insert operation
            short res = Insert(rand_value, &head);
            insOps++;
            totOps++;
        }
        else if (op == 1 && delOps < Del) {
            // Delete operation
            short res = Delete(rand_value, &head);
            delOps++;
            totOps++;
        }
        else if (memOps < Mem) {
            // Member operation
            short res = Member(rand_value, head);
            memOps++;
            totOps++;
        }
    }

    // Stop timing
    gettimeofday(&stop, NULL);
    unsigned long elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000 +
                                stop.tv_usec - start.tv_usec;

    printf("Operations completed: %d Member, %d Insert, %d Delete\n",
           memOps, insOps, delOps);
    printf("Serial execution time: %lu microseconds\n", elapsed_time);

    // Clean up memory
    destructor(head);

    return elapsed_time;
}

// Function to run multiple tests and calculate statistics
void run_performance_tests() {
    const int num_runs = 30; // For statistical significance
    unsigned long times[num_runs];

    printf("\n=== PERFORMANCE TESTING ===\n");

    for (int case_num = 1; case_num <= 3; case_num++) {
        printf("\n--- Case %d ---\n", case_num);

        unsigned long total_time = 0;
        unsigned long min_time = ULONG_MAX;
        unsigned long max_time = 0;

        for (int run = 0; run < num_runs; run++) {
            printf("Run %d/%d: ", run + 1, num_runs);
            times[run] = test_serial_run(case_num);
            total_time += times[run];

            if (times[run] < min_time) min_time = times[run];
            if (times[run] > max_time) max_time = times[run];
        }

        // Calculate average
        double avg_time = (double)total_time / num_runs;

        // Calculate standard deviation
        double variance = 0;
        for (int i = 0; i < num_runs; i++) {
            variance += (times[i] - avg_time) * (times[i] - avg_time);
        }
        variance /= num_runs;
        double std_dev = sqrt(variance);

        // Calculate 95% confidence interval
        double margin_error = 1.96 * std_dev / sqrt(num_runs);

        printf("\nCase %d Results:\n", case_num);
        printf("Average time: %.2f microseconds\n", avg_time);
        printf("Standard deviation: %.2f microseconds\n", std_dev);
        printf("Min time: %lu microseconds\n", min_time);
        printf("Max time: %lu microseconds\n", max_time);
        printf("95%% Confidence interval: %.2f ± %.2f microseconds\n",
               avg_time, margin_error);
        printf("Range: [%.2f, %.2f] microseconds\n",
               avg_time - margin_error, avg_time + margin_error);
    }
}

int main() {
    printf("Serial Linked List Performance Test\n");
    printf("====================================\n");

    // Run performance tests
    run_performance_tests();

    return 0;
}