#include<mutex>

#include"cpp11_mutex_lock.h"
#include"globals.h"
#include<iostream>

alignas(CACHELINE_SIZE) std::mutex mutex_lock;

void lock() {
  mutex_lock.lock();
}

void unlock() {
  mutex_lock.unlock();
}

void cpp11_mutex_sync(int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    useful_out(uo);
  }
}

void init_cpp11_mutex_lock() {}

void delete_cpp11_mutex_lock() {}
