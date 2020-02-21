#ifndef __BOOST_MPMC_H__
#define __BOOST_MPMC_H__ // prevent against double-inclusion

void sync_producer(int nr_iter);
// Perform nr_iter enqueue (push) operations on the shared global queue.

void sync_consumer(int nr_iter);
// Perform nr_iter de-queue (pop) operations on the shared global queue.

#endif
