#ifndef STARTER_CODE_PRIORITY_QUEUE_H
#define STARTER_CODE_PRIORITY_QUEUE_H

typedef struct priority_node {
    void *data;
    int priority;
    struct priority_node *next;
} pnode_t;

typedef struct priority_queue {
    pnode_t *head;
    pnode_t *tail;
    int n;
} pqueue_t;

pqueue_t *create_pqueue();

void enpqueue(pqueue_t *, void *, int);

void *depqueue(pqueue_t *);

void free_pqueue(pqueue_t *);

#endif //STARTER_CODE_PRIORITY_QUEUE_H

