#ifndef __CO_STACK_DRIVER_H__
#define __CO_STACK_DRIVER_H__ // prevent against double-inclusion
void sync_producer(int nr_iter);
// Perform nr_iter enqueue (push) operations on the shared global queue(stack).

void sync_consumer(int nr_iter, int tid);
// Perform nr_iter de-queue (pop) operations on the shared global queue(stack).
#endif
