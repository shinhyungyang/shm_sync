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
  // FilterLock implementation note
  //
  // if the level of any thread k is higher or equal to i, then return true
  // return false iff level of all other threads are lower than i
  //
  // In each thread, it will look at level value of other threads, and
  // it should be guaranteed that for each level varible:
  //   the storing to   the variable from different thread [happens-before]
  //   the loading from the variable on current thread
  //
  // In PetersonLock, victim variable is the commonly shared variable which
  // synchronizes the two threads before the loading of flag[0,1] variable.
  //
  // In FilterLock, however, there is no single commonly shared variable
  for (int level = 1; level < thread_number; ++level) {
    // set
    env_vars.levels[tid].v.store(level); // Read-only except for me
    // It's a conflict between threads at the same level. But we don't
    // know who is competing for the same victim[level]
    // atomically write to victims[level]
    env_vars.victims[level].v.exchange(tid);
    // evaluate
    while (higher_or_equal(tid, thread_number, level) &&
           tid == env_vars.victims[level].v.load())
      std::this_thread::yield();
  }
}

void unlock(int tid) {
  env_vars.levels[tid].v.store(0); // reset.
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
    level_k = env_vars.levels[k].v.load();
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
