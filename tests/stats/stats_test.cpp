#include"stats.h"

#include<iostream>

int main(void) {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Test 1:
  // 10 threads, 1 thread per group
  // => 10 scaling-runs 0 .. 9
  //
  //////////////////////////////////////////////////////////////////////////////

  uint32_t nr_threads = 10;
  uint32_t nr_exec    = 1;
  uint32_t nr_runs    = 10;
  initStats(nr_threads, nr_exec, nr_runs);
  for (uint32_t run = 0; run < nr_runs; run++) {
    for (uint32_t tr = 0; tr <= nr_threads; tr++) {
      enterSample(1+tr, tr, 0, run);
    }
  }

  // Collect data of all threads of a scaling run:
  range_smpl_collector col_1(nr_threads, 0, nr_threads, true);
  dumpData(false, &col_1);
  deleteSamples();


  //////////////////////////////////////////////////////////////////////////////
  //
  // Test 2:
  // 6 threads, 1 thread per group, no scaling runs
  //
  //////////////////////////////////////////////////////////////////////////////

  nr_threads = 6;
  nr_exec    = 1;
  nr_runs    = 1;
  initStats(nr_threads, nr_exec, nr_runs);
  for (uint32_t tr = 0; tr <= nr_threads; tr++) {
    enterSample(1+tr, tr, 0, 0);
  }

  range_smpl_collector col_2(nr_threads, 0, nr_threads, false);
  dumpData(false, &col_2);
  deleteSamples();


  //////////////////////////////////////////////////////////////////////////////
  //
  // Test 3:
  // 10 threads, 2 threads per group
  // => 5 scaling-runs 0 .. 4
  //
  //////////////////////////////////////////////////////////////////////////////

  nr_threads = 10;
  nr_exec    = 1;
  nr_runs    = 5;
  initStats(nr_threads, nr_exec, nr_runs);
  for (uint32_t run = 0; run < nr_runs; run++) {
    for (uint32_t tr = 0; tr <= nr_threads; tr++) {
      enterSample(1+tr, tr, 0, run);
    }
  }

  // Collect data of lower-half of threads of a scaling run:
  range_smpl_collector col_lower(nr_threads, 0, nr_threads/2, true);
  dumpData(false, &col_lower);

  // Collect data of upper-half of threads of a scaling run:
  range_smpl_collector col_upper(nr_threads, nr_threads/2, nr_threads, true);
  dumpData(false, &col_upper);
  deleteSamples();

  return 0;
}
