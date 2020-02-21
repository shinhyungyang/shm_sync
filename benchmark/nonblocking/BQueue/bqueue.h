#ifndef __B_QUEUE_H__
#define __B_QUEUE_H__ // prevent against double-inclusion

extern "C" {
#include"fifo.h"
}

void sync_producer(int nr_iter, struct queue_t* queue);
// Perform nr_iter enqueue (push) operations on the shared global queue.

void sync_consumer(int nr_iter, struct queue_t* queue);
// Perform nr_iter de-queue (pop) operations on the shared global queue.

#endif
