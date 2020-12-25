#ifndef STARTER_CODE_TIMER_H
#define STARTER_CODE_TIMER_H

#include <sys/time.h>

#define a 0.125
#define b 0.25

void timer_start(struct timeval *);

int timer_now(struct timeval *, struct timeval *);

#endif //STARTER_CODE_TIMER_H
