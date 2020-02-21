#include"globals.h"

extern "C" {
#include"fifo.h"
}

void sync_producer(int iter, struct queue_t* queue)
{
  for (int i = 0; i != iter; ++i) {
    while(enqueue(queue, i+1) != SUCCESS);
  }
}

void sync_consumer(int iter, struct queue_t* queue)
{
  int value;
  for (int i = 0; i != iter; ++i) {
    while(dequeue(queue, &value) != SUCCESS);
  }
}
