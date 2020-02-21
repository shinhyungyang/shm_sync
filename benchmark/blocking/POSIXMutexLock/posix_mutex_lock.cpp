#include<atomic>
#include<cstdlib>
#include<iostream>

#include"globals.h"
#include"posix_mutex_lock.h"

struct alignas(CACHELINE_SIZE) ENV_VARS {
  alignas(CACHELINE_SIZE) pthread_mutex_t * lock;
};

struct ENV_VARS env_vars;

inline void lock() {
  pthread_mutex_lock(env_vars.lock);
}

inline void unlock() {
  pthread_mutex_unlock(env_vars.lock);
}

void posix_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    useful_out(uo);
  }
}

void init_posix() {
  if (0 != posix_memalign((void**)(&env_vars.lock), CACHELINE_SIZE, sizeof(pthread_mutex_t))) {
    std::cerr << "posix_memalign failed. Exiting.." << std::endl;
    std::exit(0);
  }
  if (0 != pthread_mutex_init(env_vars.lock, NULL)) {
    std::cerr << "pthread_mutex_init failed. Exiting.." << std::endl;
    std::exit(0);
  }
}

void delete_posix() {
  pthread_mutex_destroy(env_vars.lock);
}
