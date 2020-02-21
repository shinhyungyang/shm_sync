#include<atomic>
#include"tatas_lock.h"
#include"globals.h"

alignas(CACHELINE_SIZE) std::atomic<bool> locked(false);

void lock() {
  do {
    while (locked.load())
      ;
    if (!locked.exchange(true))
      return;
  } while (true);
}

void unlock() {
  locked.store(false);
}

void tatas_sync(int nr_iter, unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    useful_out(uo);
  }
}

void init_tatas_lock() {}

void delete_tatas_lock() {}
