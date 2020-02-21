#ifndef __BOOST_SPSC_H__
#define __BOOST_SPSC_H__ // prevent against double-inclusion

#include<boost/lockfree/spsc_queue.hpp>

void sync_producer(int nr_iter, boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024> >* queue);
// Perform nr_iter enqueue (push) operations on the shared global queue.

void sync_consumer(int nr_iter, boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024> >* queue);
// Perform nr_iter de-queue (pop) operations on the shared global queue.

#endif
