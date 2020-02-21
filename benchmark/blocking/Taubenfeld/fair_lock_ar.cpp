#include<iostream>
#include<atomic>

#include"fair_lock.h"
#include"globals.h"

//
// Acquire-release semantics:
//
// Participating threads synchronize via the TATAS-lock variable, which
// employs acquire-release semantics. These LD/ST operations on the lock
// establish the synchronizes-with relationship via which threads can
// transport their pay-load shared data.
//
// The fairness aspect enforced by the Taubenfeld algorithm is only a wrapper
// around the TATAS lock (see procedure fair_sync() below). Because the lock
// already establishes synchronizes-with, the fairness-part only needs to
// consider ordering of its own shared state, but not the shared state (pay-
// -load) of the synchronizing threads.
//

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

  // Desired ordering from algorithm (?):
  // state[tid].state.ST(2) -> tmp = group.LD() -> state[tid].state.ST(tmp)
  state[tid].state.store(2, std::memory_order_relaxed);
  int tmp = group.load(std::memory_order_relaxed);
  // the two statements above can be executed in any order, but before the statement below
  state[tid].state.store(tmp, std::memory_order_release);
  int current_state;

  //
  // Waiting section:
  //
  for (int j = 0; j < nr_threads; j++) {
    // if statement can be executed at any place in the loop body -> relaxed load
    if (tmp != group.load(std::memory_order_relaxed)) {
      break; // process is enabled
    }
    do {
      current_state = state[j].state.load(std::memory_order_relaxed);
    } while (current_state == 2);
    // the if-statement below is data dependent on the loop above -> relaxed load
    if (current_state == 1 - tmp) {
      // different groups:
      while ((state[j].state.load(std::memory_order_relaxed) == 1 - tmp)
             && tmp == group.load(std::memory_order_relaxed)) { ; }
    }
  }
  // To prohibit that parts of "lock" are moved inside of "entry_section"
  // and vice versa -> fence acq_rel below
  //std::atomic_thread_fence(std::memory_order_acq_rel);
  // BBURG, tentative:
  //std::atomic_thread_fence(std::memory_order_seq_cst);

  // SHYANG, proposal (revised):
  /***************************************************************************
   * entry_section} {lock                   } {crit} {unlock
   *
   *                 [  C++11 load-acquire ]          [ C++11 store-release  ]
   *    #StoreLoad    LD locked; #LoadLoad             #StoreStore ST locked;
   *     #LoadLoad               #LoadStore            #LoadStore
   ***************************************************************************/

  // - In entry_section, #StoreLoad effectively prevents reordering of all
  //   stores before the barrier against "LD locked".
  // - #LoadLoad in entry_section effectly prevents reordering of all loads
  //   before the barrier with "LD locked".
  // - Unlike these reordering prevention guarantees of individual assembly
  //   memory fences in entry_section, fences "#LoadLoad;#LoadStore;" in lock
  //   and "#StoreStore;#LoadStore;" in unlock are meant to constitute C++11
  //   acquire/release semantics with regard to the atomic variable "locked".
  //   Consequently, because C++11 acquire/release semantics do not state such
  //   reordering prevention guarantees, we do not omit #LoadLoad in
  //   entry_section even in the presence of #LoadLoad in lock.
  //
  // Define a macro to insert inline assembly code which effectively prevents
  // any store or load operations performed inside the critical section (i.e.,
  // parts of "lock") from moving inside of "entry_section"
#if defined (__x86_64__)
  asm volatile("mfence" ::: "memory");
#elif defined (__aarch64__)
  asm volatile("dmb ish" ::: "memory");
#else
  #error "Unsupported architecture"
#endif
}

inline void exit_section(int tid) {
  //
  // A thread that comes here has committed a release-store inside of unlock()
  // We exploit this fact here.
  //

  // No acquire_release fence at this point. Thus LD/ST operations from this
  // exit_section may be moved into unlock(), or even further into the critical
  // section.  The load_acquire inside lock() is the fence that blocks further
  // movements.
  // 
  // From the paper:
  // (1) "We notice that the algorithm is also correct when we
  // replace the order of lines 9 and 10, allowing process i to write the group
  // bit immediately before it enters its fair section."
  // (2) " The order of lines 10 and 11 is crucial for correctness."
  //
  // We fulfill (1) by the load_aquire inside lock(), and we fulfull (2) by the
  // release-store at the end of this exit routine.
 
  int store_val = 1 - state[tid].state.load(std::memory_order_relaxed);
  // Releaxed LD; ordered happens-before following store.

  group.store(store_val, std::memory_order_relaxed);
  // Cannot be reordered with previous LD because of LD->ST dependency
  // -> relaxed store

  state[tid].state.store(3, std::memory_order_release);
  // Force previous ST before this ST: -> release-store
}

//
// Mutual exlusion lock:
//
// Fair threads arrive concurrently at the lock and require synchronization.
// Acquire-release semantics apply (no further relaxation possible).
//
inline void lock() {
  do {
    while (locked.load(std::memory_order_relaxed)) { ; }
    if (!locked.exchange(true, std::memory_order_acquire))
      return;
  } while (true);
}

inline void unlock() {
  locked.store(false, std::memory_order_release);
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
