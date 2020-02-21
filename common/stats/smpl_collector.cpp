#include"smpl_collector.h"
#include"globals.h"

#include<iostream>
#include<cstdlib>
#include<cassert>

void error_exit(const std::string & msg);

//
// range_smpl_collector:
//

range_smpl_collector::range_smpl_collector(uint32_t population_size,
                                           uint32_t start_index,
                                           uint32_t stop_index,
                                           bool is_scaling)
{
  this->population_size = population_size;
  this->start_index = start_index;
  this->stop_index = stop_index;
  this->nr_samples = stop_index - start_index;
  this->is_scaling = is_scaling;
  // Allocate space to hold one execution of samples:
  uint64_t size_in_bytes = nr_samples * sizeof(long long);
  sample = (long long *) aligned_alloc (CACHELINE_SIZE, size_in_bytes);
  if (NULL == sample) {
    error_exit("Error: alloc. of sampling memory failed");
  }
}

range_smpl_collector::~range_smpl_collector() {
  free(sample);
}

long long * range_smpl_collector::get_samples(uint32_t run,
                                              long long * row)
{
  //assert((is_scaling || run == 0) && "Index of non-scaling run must be zero");
  assert(run < nr_samples && "Index of run out of collector's range");
  uint32_t s_upper = is_scaling?run+1:stop_index-start_index;
  for (uint32_t smpl_idx = 0; smpl_idx < s_upper; smpl_idx++) {
    sample[smpl_idx] = row[start_index + smpl_idx];
  }
  return sample;
}

uint32_t range_smpl_collector::get_nr_samples(uint32_t run) {
  //assert((is_scaling || run == 0) && "Index of non-scaling run must be zero");
  if (is_scaling) {
    // Runs are zero-indexed. The number of threads (and hence samples) in a run
    // is thus run+1.
    assert(run < nr_samples && "Index of run out of collector's range"); 
    return run + 1;
  } else {
    return nr_samples;
  }
}

bool range_smpl_collector::includes(uint32_t run, uint32_t sample_nr) {
  //
  // Check whether within the sampling population. Out-of-bounds constitutes
  // an error.
  //
  if (sample_nr >= population_size) {
    error_exit("Error: sample_nr out of range");
  }
  //
  // Check whether within the collector's range:
  //
  if ((sample_nr < start_index) || (sample_nr >= stop_index)) {
    // Sample out of collector's range:
    return false;
  }
  //
  // Reaching here means the sample is within the collector's range.
  // If this is a scaling run, then the sample must additionally be
  // included in this run:
  //
  if (is_scaling) {
    return sample_nr <= start_index + run;
  }
  return true;
}

void range_smpl_collector::dumpMask() {
  std::cout << "Threads in mask: [" << start_index << ".." << stop_index << "[\n";
}


void error_exit(const std::string & msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}
