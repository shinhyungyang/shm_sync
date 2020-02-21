extern "C" {
#include"fifo.h"
}
#include"bqueue.h"
#include"globals.h"
#include"barrier.h"
#include"stats.h"
#include"log.h"

#include<cxxopts.hpp>
#include<papi.h>

#include<thread>
#include<cstdlib>
#include<cstring>
#include<iostream>

//
// Producer thread function:
//

void f_prod(int tid, int nr_pairs, int nr_iter, int nr_exec, bool sc_runs, struct queue_t* queue) {

  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    std::cerr << "PAPI_thread_init failed (tid: " << tid << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  int nr_pairs_lower_bound = 1;
  int iter_per_thread = 0;

  if (!sc_runs) {
    // No scaling runs from 1 .. nr_pairs:
    nr_pairs_lower_bound = nr_pairs;
    iter_per_thread = (int)(nr_iter / nr_pairs);
    if (tid < (nr_iter % nr_pairs)) iter_per_thread ++;
  }

  for (int run = 0; run <= nr_pairs - nr_pairs_lower_bound; run++) {
    if (sc_runs) {
      iter_per_thread = (int)(nr_iter / (run + 1) );
      if (tid < (nr_iter % (run + 1))) iter_per_thread ++;
    }

    for (int exec = 0; exec < nr_exec; exec++) {
      barrier_wait(tid);
      if (!sc_runs || tid <= run ) {
        //
        // Thead ``tid'' is participating in run ``run'':
        //
        long long nsec_start = PAPI_get_real_nsec();

        sync_producer(iter_per_thread, queue);

        long long nsec_end = PAPI_get_real_nsec();
        enterSample(nsec_end - nsec_start, tid, exec, run);
      }
    }
  }
}


//
// Consumer thread function:
//

void f_cons(int tid, int nr_pairs, int nr_iter, int nr_exec, bool sc_runs, struct queue_t* queue) {

  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    std::cerr << "PAPI_thread_init failed (tid: " << tid << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  int nr_pairs_lower_bound = 1;
  int iter_per_thread = 0;

  if (!sc_runs) {
    // No scaling runs from 1 .. nr_pairs:
    nr_pairs_lower_bound = nr_pairs;
    iter_per_thread = (int)(nr_iter / nr_pairs);
    if (tid < (nr_iter % nr_pairs)) iter_per_thread ++;
  }

  for (int run = 0; run <= nr_pairs - nr_pairs_lower_bound; run++) {
    if (sc_runs) {
      iter_per_thread = (int)(nr_iter / (run + 1) );
      if ((tid - nr_pairs) < (nr_iter % (run + 1))) iter_per_thread ++;
    }

    for (int exec = 0; exec < nr_exec; exec++) {
      barrier_wait(tid);
      if (!sc_runs || tid - nr_pairs <= run ) {
        //
        // Thead ``tid'' is participating in run ``run'':
        //
        long long nsec_start = PAPI_get_real_nsec();

        sync_consumer(iter_per_thread, queue);

        long long nsec_end = PAPI_get_real_nsec();
        enterSample(nsec_end - nsec_start, tid, exec, run);
      }
    }
  }
}


//
// Main:
//

int main(int argc, char *argv[]) {
  int nr_exec = 1;
  int nr_iter = 10000000;
  int nr_pairs = 1;
  bool sc_runs = false;
  bool ovw_stats = false;
  int tid;

  //
  // Parse command-line arguments:
  //
  cxxopts::Options options("Bqueue",
                           "Bqueue single-writer single-consumer"
                           "synchronization algorithm");
  options
    .add_options()
      ("e,executions", "Number of executions", cxxopts::value<int>(nr_exec))
      ("i,iterations", "Number of iterations", cxxopts::value<int>(nr_iter))
      ("t,threadpairs", "Number of prod/cons thread-pairs",
          cxxopts::value<int>(nr_pairs))
      ("r,runs", "Scaling runs, 1..t thread-pairs",
          cxxopts::value<bool>(sc_runs))
      ("o,ovw", "Overview statistics only", cxxopts::value<bool>(ovw_stats))
      ("h,help", "Usage information")
      ("positional",
       "Positional arguments: these are the arguments that are entered "
       "without an option", cxxopts::value<std::vector<std::string>>())
    ;

  options.parse_positional("positional");
  auto result = options.parse(argc, argv);

  if (result.count("h") || result.count("positional")) {
    std::cerr << options.help({"", "Group"}) << std::endl;
    exit(EXIT_FAILURE);
  }

  if (2 * nr_pairs > MAX_ALLOC_NR_THREADS) {
    std::cerr << "ERROR: requested number of threads exceeds compile-time max"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Initialize PAPI:
  //
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    std::cerr << "PAPI_library_init failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Initialize barrier:
  //
  barrier_init(2 * nr_pairs);

  //
  // Initialize sampling component:
  //
  initStats(2 * nr_pairs, nr_exec, sc_runs?nr_pairs:1);
  
  //
  // Create/join threads for benchmark:
  // BQueue requires queue_t queues for each pair (nr_pairs number of queues in total)
  // and, they are initialized before execution.
  //
  std::thread tids[2 * nr_pairs];

  queue_t queues[nr_pairs];
  for(int i = 0; i < nr_pairs; ++i){
    queue_init(&(queues[i]));
  }

  for (tid = 0; tid<nr_pairs; tid++) {
    //
    // For the sample collection, we align the threads into an array of
    // producers followed by the consumers:
    //
    // p_1 p_2 ... p_n c_1 c_2 ... c_n
    // |---- p_id ---| |---- c_id ---|
    //
    // The p_id and c_id is the thread-ID under which a thread will enter its
    // samples with the sampling component.
    //
    int p_id = tid;
    int c_id = tid + nr_pairs;
    tids[p_id] = std::thread(f_prod, p_id, nr_pairs,
                            nr_iter, nr_exec, sc_runs, &queues[tid]);
    tids[c_id] = std::thread(f_cons, c_id, nr_pairs,
                            nr_iter, nr_exec, sc_runs, &queues[tid]);
  }
  for (tid = 0; tid<nr_pairs; tid++) {
    tids[tid].join();
    tids[tid + nr_pairs].join();
  }

  //
  // Collect statistics from sampled data:
  //

  // Lower-half of threads (the producers):
  range_smpl_collector col_lower(2 * nr_pairs, 0, nr_pairs, sc_runs);
  dumpData(ovw_stats, &col_lower, "BQueue_producers.txt"); 

  // Upper-half of threads (the consumers):
  range_smpl_collector col_upper(2 * nr_pairs, nr_pairs, 2 * nr_pairs, sc_runs);
  dumpData(ovw_stats, &col_upper, "BQueue_consumers.txt"); 

  //
  // Cleanup and exit:
  //
  deleteSamples();
  return 0;
}
