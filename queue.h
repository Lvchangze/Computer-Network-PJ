#ifndef STARTER_CODE_QUEUE_H
#define STARTER_CODE_QUEUE_H

typedef struct node {
    void *data;
    struct node *next;
} node;

typedef struct queue {
    node *head;
    node *tail;
    int n;
} queue;

queue *create_queue();

void init_queue(queue *);

void free_queue(queue *, int);

void enqueue(queue *, void *);

void *dequeue(queue *);

#endif //STARTER_CODE_QUEUE_H
