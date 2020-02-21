#include"globals.h"

#include<boost/lockfree/queue.hpp>

static boost::lockfree::queue<int> queue(128);
// Shared global MPMC queue


void sync_producer(int nr_iter) {
  for (int i = 0; i < nr_iter; ++i) {
    while (!queue.push(i)) { ; }
  }
}

void sync_consumer(int nr_iter) {

  int value = 0;

  for (int i = 0; i < nr_iter; ++i) {
    while (!queue.pop(value)) { ; }
  }
}
