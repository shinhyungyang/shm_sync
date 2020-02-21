#include<atomic>
#include<cstdlib>
#include<iostream>
#include<papi.h>
#include<thread>

#include"cxxopts.hpp"

#include"barrier.h"
#include"globals.h"
#include"stats.h"
#include"posix_mutex_lock.h"
#include"timer.h"

template<typename F>
auto Decision(F delay, unsigned long sec) -> decltype(delay)
{
  auto func = ( sec > 0? delay : [](unsigned long sec){});
  return func;
}

void thread_f(int tid, int nr_threads, int nr_iter, int nr_exec, bool sc_runs, 
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out) {

  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    std::cerr << "PAPI_thread_init failed (tid: " << tid << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  int nr_thrds_lower_bound = 1;
  int iter_per_thread = 0;
  
  if (!sc_runs) {
    // No scaling runs from 1 .. nr_threads:
    nr_thrds_lower_bound = nr_threads;
    iter_per_thread = (int) (nr_iter / nr_threads);
    if (tid < (nr_iter % nr_threads)) iter_per_thread ++;
  }

  for (int run = 0; run <= nr_threads - nr_thrds_lower_bound; run++) {
    if (sc_runs) {
      iter_per_thread = (int)(nr_iter / (run + 1) );
      if (tid < (nr_iter % (run + 1))) iter_per_thread ++;
    }
    for (int exec = 0; exec < nr_exec; exec++) {
      barrier_wait(tid);
      if (!sc_runs || tid <= run) {
        //
        // Thead ``tid'' is participating in run ``run'':
        //
        long long nsec_start = PAPI_get_real_nsec();

        posix_sync(tid, nr_threads, iter_per_thread, ui, uo, useful_in, useful_out);

        long long nsec_end = PAPI_get_real_nsec();
        enterSample(nsec_end - nsec_start, tid, exec, run);
      }
    }
  }
}

int main(int argc, char *argv[]) {

  int nr_exec = 1;
  int nr_iter = 10000000;
  int nr_threads = 1;
  bool sc_runs = false;
  bool ovw_stats = false;
  int tid;

  unsigned long uin = 0;
  unsigned long uout = 0;
  //
  // Parse command-line arguments:
  //
  cxxopts::Options options("POSIX_mutexlock", "Fair mutual exclusion algorithm.");
  options.
    add_options()
    ("e,executions", "Number of executions", cxxopts::value<int>(nr_exec))
    ("i,iterations", "Number of iterations", cxxopts::value<int>(nr_iter))
    ("t,threads", "Number of threads", cxxopts::value<int>(nr_threads))
    ("r,runs", "Scaling runs, 1..t threads", cxxopts::value<bool>(sc_runs))
    ("u,uin","amount of nsec to perform useful work inside critical section",cxxopts::value<unsigned long>(uin))
    ("y,uout","amount of nsec to perform useful work outside critical section",cxxopts::value<unsigned long>(uout))
    ("o,ovw", "Overview statistics only", cxxopts::value<bool>(ovw_stats))
    ("h,help", "Usage information")
    ("positional",
     "Positional arguments: these are the arguments that are entered "
     "without an option",cxxopts::value<std::vector<std::string>>());

  options.parse_positional("positional");
  auto result = options.parse(argc, argv);

  if (result.count("h") || result.count("positional")) {
    std::cerr << options.help({"", "Group"}) << std::endl;
    exit(EXIT_FAILURE);
  }

  if (nr_threads > MAX_ALLOC_NR_THREADS) {
    std::cerr << "ERROR: requested number of threads exceeds compile-time max"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  //
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
  barrier_init(nr_threads);

  //
  // Initialize sampling component:
  //
  initStats(nr_threads, nr_exec, sc_runs ? nr_threads : 1);

  //
  // Initialize fair mutual exclusion lock:
  //
  init_posix();
  
  //
  // Calibrate jiffy
  //
  calibrate_delay();

  //
  // Initialize delay_loop function to be used 
  //
  auto uin_func =  Decision(useful_work_ns,uin); // nano-second delay (useful_inside)
  auto uout_func = Decision(useful_work_ns,uout); // micro-second delay (useful_outside)

  //
  // Create/join threads for benchmark:
  //
  std::thread tids[nr_threads];
  for (tid = 0; tid < nr_threads; tid++) {
    tids[tid] =
      std::thread(thread_f, tid, nr_threads, nr_iter, nr_exec, sc_runs,
          uin, uout, uin_func, uout_func);
  }
  for (tid = 0; tid < nr_threads; tid++) {
    tids[tid].join();
  }

  //
  // Cleanup and exit:
  //
  dumpData(ovw_stats);
  delete_posix();
  return 0;
}
