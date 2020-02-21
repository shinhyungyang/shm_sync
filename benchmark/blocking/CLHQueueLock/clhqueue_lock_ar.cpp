#include<iostream>
#include<atomic>
#include"clhqueue_lock.h"
#include"fences.h"
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
  myNode->locked.store(true, std::memory_order_relaxed);
  mem_fence(fences::ss);
  // In conjunction with below substitution for memory_order_consume:
  // Tentative substitute for memory_order_release
  myPred = tail.exchange(myNode, std::memory_order_relaxed);
  // Tentative substitute for memory_order_consume which is discouraged for
  // C++ 2017 (see P0371R1). Re-visit once the revision for C++ 2020 has
  // materialized

  while (myPred->locked.load(std::memory_order_acquire)) {} 
}

void unlock() {
  myNode->locked.store(false, std::memory_order_release);
  myNode = myPred; // reuse unassigned myPred object
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
