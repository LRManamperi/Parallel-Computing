#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Node structure
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Function prototypes
int Member(int value, struct list_node_s* head_p);
int Insert(int value, struct list_node_s** head_pp);
int Delete(int value, struct list_node_s** head_pp);
void destructor(struct list_node_s* head);

#endif
