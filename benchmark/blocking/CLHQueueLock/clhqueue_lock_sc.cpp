#include<iostream>
#include<atomic>
#include"clhqueue_lock.h"
#include"globals.h"

struct alignas(CACHELINE_SIZE) aligned_node {
  std::atomic<bool> locked;
};

alignas(CACHELINE_SIZE) std::atomic<aligned_node *> tail;
aligned_node common_node;

alignas(CACHELINE_SIZE) thread_local aligned_node * myNode = nullptr;
alignas(CACHELINE_SIZE) thread_local aligned_node * myPred = nullptr;
thread_local aligned_node * myAlloc = nullptr;

void lock() {
  myNode->locked = true;
  myPred = tail.exchange(myNode);
  while (myPred->locked) {}
}

void unlock() {
  myNode->locked = false;
  myNode = myPred;
}

void clhqueue_sync(int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for(int iter=0; iter != nr_iter; ++iter){
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    useful_out(uo);
  }
}

void init_clhqueue_lock() {
}

void init_per_exec_global() {
  tail.store(&common_node);
  tail.load()->locked = false;
}

void delete_clhqueue_lock() {
}

void init_per_exec_per_thread() {
  myAlloc = (aligned_node *) aligned_alloc (CACHELINE_SIZE, sizeof(aligned_node));

  if (NULL == myAlloc) {
    std::cerr << "Error: allocation of synchronization shared data failed"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  myNode = myAlloc;
}

void delete_per_thread() {
  free(myAlloc);
}
