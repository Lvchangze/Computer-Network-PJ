#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "priority_queue.h"

pqueue_t *create_pqueue() {
    pqueue_t *q = malloc(sizeof(pqueue_t));
    q->head = q->tail = NULL;
    q->n = 0;
    return q;
}

void free_pqueue(pqueue_t *q) {
    while (q->n) {
        depqueue(q);
    }
    free(q);
}

void enpqueue(pqueue_t *q, void *data, int p) {
    pnode_t *n = malloc(sizeof(pnode_t));
    n->data = data;
    n->priority = p;
    n->next = NULL;
    if (q->head == NULL) {
        q->head = n;
        q->tail = n;
    } else {
        if (n->priority < q->head->priority) {
            n->next = q->head;
            q->head = n;
        } else if ((n->priority) > (q->tail->priority)) {
            q->tail->next = n;
            q->tail = n;
        } else {
            pnode_t *pre = q->tail;
            pnode_t *cur;
            for (cur = pre->next; cur != NULL; cur = cur->next) {
                if (cur->priority > n->priority) {
                    break;
                } else {
                    pre = cur;
                }
            }
            pre->next = n;
            n->next = cur;
        }
    }
    q->n += 1;
}

void *depqueue(pqueue_t *q) {
    if (q->n == 0) { // null to ret
        return NULL;
    }
    pnode_t *head = q->head;
    void *ret = head->data;
    q->head = head->next;
    q->n -= 1;
    free(head);
    if (q->n == 0) {
        q->tail = NULL;
    }
    return ret;
}

