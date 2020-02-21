#include<atomic>
#include"arraylock.h"
#include"globals.h"

struct alignas(CACHELINE_SIZE) aligned_bool {
  std::atomic<bool> b;
};

aligned_bool flags[MAX_NR_THREADS];

alignas(CACHELINE_SIZE) std::atomic<long long> tail;

alignas(CACHELINE_SIZE) thread_local int thread_lock_slot;

inline void lock(int nr_threads) {
  thread_lock_slot = (tail++) % nr_threads;
  while (!flags[thread_lock_slot].b.load()) {
    ;
  }
}

inline void unlock(int nr_threads) {
  flags[thread_lock_slot].b.store(false);
  flags[(thread_lock_slot + 1) % nr_threads].b.store(true);
}

void arraylock_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo,delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    lock(nr_threads);
    useful_in(ui);
    unlock(nr_threads);
    useful_out(uo);
  }
}

void init_arraylock(int nr_threads) {
  tail = 0;
  for (int i = 0; i < nr_threads; ++i) {
    flags[i].b.store(false);
  }
  flags[tail].b.store(true);
}

void delete_arraylock() {}
