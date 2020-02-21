#include<iostream>
#include<atomic>

#include"fair_lock.h"
#include"globals.h"

/*

Algorithm 1 from Gadi Taubenfeld, Fair synchronization, J. Parallel Distrib.
Comput., Vol. 97, pp. 1-10, 2016.

group: atomic bit; the initial value of the group bit is immaterial.
state[1..n]: array of atomic registers, which range over {0, 1, 2, 3}
Initially forall i : 1 <= i <= n : statei = 3 // processes are inactive
1 state_i := 2 // begin doorway
2 state_i := group // choose group and end doorway
3 for j = 1 to n do // begin waiting
4   if (state_i /= group) then break fi // process is enabled
5   await state_j /= 2
6   if state_j = 1 − state_i // different groups
7   then await (state_j /= 1 − state_i) v (state_i /= group) fi
8 od // end waiting
9 fair section
10 group := 1 − state_i // begin exit
11 state_i := 3

*/

alignas(CACHELINE_SIZE) std::atomic<int> group(0);

struct alignas(CACHELINE_SIZE) aligned_state {
  std::atomic<int> state;
};

aligned_state * state = NULL;

alignas(CACHELINE_SIZE) std::atomic<bool> locked(false);
// TAS mutual exclusion lock

inline void entry_section(int tid, int nr_threads) {
  //
  // Doorway section:
  //
  state[tid].state = 2;
  state[tid].state = group.load();
  //
  // Waiting section:
  //
  for (int j = 0; j < nr_threads; j++) {
    if (state[tid].state != group) {
      break; // process is enabled
    }
    while (state[j].state == 2) { ; }
    if (state[j].state == 1 - state[tid].state) {
      // different groups:
      while ((state[j].state == 1 - state[tid].state)
             && state[tid].state == group) { ; }
    }
  }
}

inline void exit_section(int tid) {
  group = 1 - state[tid].state;
  state[tid].state = 3;
}

inline void lock() {
  do {
    while (locked.load(std::memory_order_relaxed)) { ; }
    if (!locked.exchange(true))
      return;
  } while (true);
}

inline void unlock() {
  locked.store(false);
}

void fair_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {
  for (int iter = 0; iter != nr_iter; ++iter) {
    // Fair synchronization (entry):
    entry_section(tid, nr_threads);
    // Mutual exclusion:
    lock();
    useful_in(ui);
    unlock();
    // Fair synchronization (exit):
    exit_section(tid);
    useful_out(uo);
  }
}

void init_fair_lock(int nr_threads) {
  state = (aligned_state *) aligned_alloc (CACHELINE_SIZE,
                                           nr_threads * sizeof(aligned_state));
  if (NULL == state) {
    std::cerr << "Error: allocation of synchronization shared data failed"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  for (int tid = 0; tid<nr_threads; tid++) {
    state[tid].state = 3;
  }
}

void delete_fair_lock() {
  free(state);
}
