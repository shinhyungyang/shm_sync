#include<thread>
#include<atomic>
#include<iostream>

#include"globals.h"
#include"filter_lock.h"

struct alignas(CACHELINE_SIZE) LEVELS {
  std::atomic<int> v;
};
struct alignas(CACHELINE_SIZE) VICTIMS {
  std::atomic<int> v;
};

struct alignas(CACHELINE_SIZE) ENV_VARS {
  alignas(CACHELINE_SIZE) struct LEVELS *levels;
  alignas(CACHELINE_SIZE) struct VICTIMS *victims;
};
struct ENV_VARS env_vars;

void lock(int tid, int thread_number) {
  for (int level = 1; level < thread_number; ++level) {
    env_vars.levels[tid].v.store(level, std::memory_order_release); // set mine
    env_vars.victims[level].v.exchange(
        tid, std::memory_order_acq_rel); // create "synchronizes-with"
    while (higher_or_equal(tid, thread_number, level) &&
           tid == env_vars.victims[level].v.load(std::memory_order_relaxed))
      std::this_thread::yield();
  }
}

void unlock(int tid) {
  env_vars.levels[tid].v.store(0, std::memory_order_release); // set mine
}

// tid: current thread's integer identifier
// thread_number: total number of worker threads
// level: current thread's level
// returns 1 if there exists any thread k that has level higher or equal to
// the level of current thread
int higher_or_equal(int tid, int thread_number, int level) {
  int level_k = 0;
  int ret = 0;
  for (int k = 0; k < thread_number; ++k) {
    if (k == tid)
      continue;
    // Tentative substitute for memory_order_consume which is discouraged for
    // C++ 2017 (see P0371R1). Re-visit once the revision for C++ 2020 has
    // materialized
    level_k =
        env_vars.levels[k].v.load(std::memory_order_relaxed); // read other's
    if (level_k >= level) {
      ret = 1;
      break;
    }
  }
  return ret;
}

void filter_lock(int tid, int thread_number, int iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int i = 0; i != iter; ++i) {
    lock(tid, thread_number);
    useful_in(ui);
    // critical section
    unlock(tid);
    useful_out(uo);
  }
}

void init_filter_lock(int thread_number) {
  if (0 != posix_memalign((void **)(&env_vars.levels), CACHELINE_SIZE,
                          sizeof(struct LEVELS) * thread_number)) {
    std::cout << "ERROR: posix_memalign failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (0 != posix_memalign((void **)(&env_vars.victims), CACHELINE_SIZE,
                          sizeof(struct VICTIMS) * thread_number)) {
    std::cout << "ERROR: posix_memalign failed" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void delete_filter_lock() {
  std::free(env_vars.levels);
  std::free(env_vars.victims);
}

void reset_filter_lock(int tid) {
  env_vars.levels[tid].v = 0;
  env_vars.victims[tid].v = 0;
}
