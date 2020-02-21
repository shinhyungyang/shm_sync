#include"globals.h"

#include<boost/lockfree/spsc_queue.hpp>


void sync_producer(int iter, boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024> >* queue)
{
  for (int i = 0; i != iter; ++i) {
    while(!queue->push(i));
  } 
}

void sync_consumer(int iter, boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024> >* queue)
{
  int value;
  for (int i = 0; i != iter; ++i) {
    while(!queue->pop(value));
    // Read value to enforce core-to-core queue data transfer
    if (value == -1) {
      return;
    }
  } 
}
