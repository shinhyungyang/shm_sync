#include<atomic>
#include<cstdlib>

#include"globals.h"
#include"taslock.h"

struct alignas(CACHELINE_SIZE) ENV_VARS {
  alignas(CACHELINE_SIZE) struct ptime *thread_vars;
  alignas(CACHELINE_SIZE) std::atomic_flag locked;
};

struct ENV_VARS env_vars;
// TAS mutual exclusion lock

inline void lock() {
  while (env_vars.locked.test_and_set(std::memory_order_acquire)) {
    ;
  }
}

inline void unlock() { env_vars.locked.clear(std::memory_order_release); }

void taslock_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    useful_out(uo);
  }
}

void init_taslock() { env_vars.locked.clear(); }

void delete_taslock() {}
