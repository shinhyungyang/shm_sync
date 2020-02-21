#include<atomic>
#include<thread>
#include<iostream>
#include<queue>

#include"globals.h"
#include"peterson_lock.h"

struct alignas(CACHELINE_SIZE) ENV_VARS {
  alignas(CACHELINE_SIZE) std::atomic<int> flag0;
  alignas(CACHELINE_SIZE) std::atomic<int> flag1;
  alignas(CACHELINE_SIZE) std::atomic<int> turn;
};
struct ENV_VARS env_vars;

void lock(int tid) {
  if (0 == tid) {
    env_vars.flag0.store(1);
    env_vars.turn.exchange(1);

    while (env_vars.flag1.load() && 1 == env_vars.turn.load())
      std::this_thread::yield();
  } else {
    env_vars.flag1.store(1);
    env_vars.turn.exchange(0);

    while (env_vars.flag0.load() && 0 == env_vars.turn.load())
      std::this_thread::yield();
  }
}

void unlock(int tid) {
  if (0 == tid) {
    env_vars.flag0.store(0);
  } else {
    env_vars.flag1.store(0);
  }
}

void peterson_lock(int tid, int iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int i = 0; i != iter; ++i) {
    lock(tid);
    useful_in(ui);
    // critical section
    unlock(tid);
    useful_out(uo);
  }
}

void reset_peterson_lock() {
  env_vars.flag0 = 0;
  env_vars.flag1 = 0;
  env_vars.turn = 0;
}
