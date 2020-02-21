#include<numeric>
#include<vector>
#include<algorithm>
#include<cmath>
#include<cstdint>
#include<cstring>
#include<cassert>
#include<iostream>
#include<iomanip>
#include<fstream>

#include"stats.h"
#include"globals.h"
#include"smpl_collector.h"
#include"color_term.h"

struct result_t {
  long long max = 0;
  long long mean = 0;
  long long stdev = 0;
  double cv = 0.0;         // coefficient of variation
  long long median = 0;
};

long long * sample = nullptr;
// Thread-granular profiling data per execution. We flatten the 3D array
// [runs] X [executions] X [threads] into a 1D array in row-major layout to
// explicate all address computations (for efficiency). The size of the array
// is determined by command-line parameters and thus heap-allocated upon
// program start-up. No dynamic memory allocation happens during profiling.

std::vector< std::vector<result_t *> * > res_exec;
// Profiling data per execution aggregated over all threads of the execution.
// The outer vector indexes into the scaling-runs (1 thread, 2 threads, ...).
// Order: run 0 -> run N.

std::vector<result_t *> res_run;
// Profiling data aggregated over all executions of a run
// Order: run 0 -> run N.

uint32_t nr_threads = 0;
// Number of threads submitting sampling data. This may vary across
// scaling runs. nr_threads is thus the upper bound.

uint32_t nr_executions = 0;
// Number of executions

uint32_t nr_runs = 0;
// Number of scaling runs

int numWidth(double num);
void printSep(std::ostream& ofl, int cols, char sep = '-');
void deleteDump();

void dumpRawData(const char * f_name);
// Dump raw sample data to file f_name. Used for debugging.

void computeSampleWidths(int * width, int run, bool initialize = true);
// Compute the maximum width of each column of samples of run ``run'' and
// store in array ``width''. Initialization of this array depends on variable
// ``initialize''.

void doStats(long long * nsec, int nr_elements, result_t * r) {
  uint64_t sum = std::accumulate(nsec, nsec + nr_elements, 0LL);
  r->mean = sum / nr_elements;
  std::vector<double> diff(nr_elements);
  std::transform(nsec, nsec + nr_elements, diff.begin(), [r](long long x){return x - r->mean; });
  double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  r->stdev = std::sqrt(sq_sum / nr_elements);
  r->cv = 1.0 * r->stdev / r->mean;
  r->max = *std::max_element(nsec, nsec + nr_elements);
}

void initStats(uint32_t nr_threads, uint32_t nr_exec, uint32_t nr_runs) {
  ::nr_threads = nr_threads;
  ::nr_executions = nr_exec;
  ::nr_runs = nr_runs;
  //
  // Allocate sampling-data memory:
  //
  uint64_t size_in_bytes = nr_threads * nr_executions * nr_runs
                           * sizeof(long long);
  sample = (long long *) aligned_alloc (CACHELINE_SIZE, size_in_bytes);
  if (nullptr == sample) {
    std::cerr << "Error: alloc. of sampling memory failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  //
  // Initialize to zero and thereby touch all pages to minimize page-faults
  // during profiling:
  //
  memset((void *)sample, 0, size_in_bytes);
}

void enterSample(long long s, uint32_t tid, uint32_t nr_exec, uint32_t nr_run) {
  assert(nullptr != sample && "Sampling not initialized!");
  assert(nr_exec < nr_executions && "Memory overrun, (executions)");
  assert(nr_run < nr_runs && "Memory overrun, (runs)");
  uint64_t samples_per_run = ::nr_threads * ::nr_executions;
  uint64_t s_index = samples_per_run * nr_run
                     + ::nr_threads * nr_exec
                     + tid;
  sample[s_index] = s;
}

void aggregateStatistics(smpl_collector & collector) {
  for (uint32_t run = 0; run < nr_runs; run++) {
    std::vector<result_t *> * v = new std::vector<result_t *>();
    // Compute aggregate information per scaling run:
    uint32_t run_offset = nr_executions * nr_threads * run;
    for (uint32_t exec = 0; exec < nr_executions; exec++) {
      //
      // Compute aggregate information per execution:
      //
      uint32_t exec_offset = run_offset + exec * nr_threads;
      //int nr_participants = nr_runs>1?run:nr_threads;
      long long * samples = collector.get_samples(run,
                                                  &sample[exec_offset]); 
      uint32_t nr_samples = collector.get_nr_samples(run);
      result_t * res_p = new result_t();
      if (nr_samples > 0) {
        doStats(samples, nr_samples, res_p);
      } else {
        std::cerr << "Warning: run with zero samples detected!\n";
      }
      v->push_back(res_p);
    }
    res_exec.push_back(v);
    //
    // Compute aggregate information over all executions of a scaling run:
    //
    long long all_max[nr_executions];
    for (uint32_t exec = 0; exec < nr_executions; exec++) {
      all_max[exec] = (*v)[exec]->max;
    }
    auto overall = new result_t();
    doStats(all_max, nr_executions, overall);
    std::sort(all_max, all_max + nr_executions);
    if (nr_executions % 2 == 0) {
      double temp = all_max[nr_executions/2 - 1] + all_max[nr_executions/2];
      overall->median = temp / 2;
    } else {
      overall->median = all_max[nr_executions/2];
    }
    res_run.push_back(overall);
  }
}

void dump_details_of_run(int run, smpl_collector & col, std::ostream& ofl) {
  std::vector<result_t *> * r_exes = res_exec[run];
  result_t * r_run = res_run[run];
  const int run_offset = run * nr_executions * nr_threads;
  // Determine max width of each column in the output:
  int w_exec = numWidth(nr_executions);
  int w_max = 0;
  int w_mean = 0;
  int w_stdev = 0;
  int w_cv = 0;
  for (uint32_t exec = 0; exec < nr_executions; exec++) {
    w_max = std::max(w_max, numWidth((*r_exes)[exec]->max));
    w_mean = std::max(w_mean, numWidth((*r_exes)[exec]->mean));
    w_stdev = std::max(w_stdev, numWidth((*r_exes)[exec]->stdev));
    w_cv = std::max(w_cv, numWidth((*r_exes)[exec]->cv));
  }
  w_exec = std::max(3, w_exec);   // "EXE"
  w_max  = std::max(3, w_max);    // "MAX"
  w_mean = std::max(4, w_mean);   // "MEAN"
  w_stdev = std::max(5, w_stdev); // "STDEV"
  w_cv = 9;                       // "CV"
  int width[nr_threads];
  computeSampleWidths(width, run, true);
  int line_width = w_exec
                   + w_max
                   + w_mean 
                   + w_stdev 
                   + w_cv 
                   + std::accumulate(width, width + nr_threads, 0) // samples 
                   + 4 + nr_threads // column separators " "
                   ;
  bool use_color = isTerminalStream(ofl) && supportsColor();
  //
  // Print output per execution:
  //
  ofl.precision(3);
  if (::nr_runs > 1) {
    // Output run's label:
    printSep(ofl, line_width, '*');
    ofl << "RUN " << run << ":\n";
    printSep(ofl, line_width, '*');
  } else {
    printSep(ofl, line_width);
  }
  ofl << std::left << std::setw(w_exec) << "EXE" << " ";
  ofl << std::left << std::setw(w_max) << "MAX" << " ";
  ofl << std::left << std::setw(w_mean) << "MEAN" << " ";
  ofl << std::left << std::setw(w_stdev) << "STDEV" << " ";
  ofl << std::left << std::setw(w_cv) << "CV" << " SAMPLES:\n";
  printSep(ofl, line_width);
  for (uint32_t exec = 0; exec < nr_executions; exec++) {
    // Aggregated statistics of execution:
    ofl << std::right << std::setw(w_exec) << exec << " ";
    ofl << std::right << std::setw(w_max) << (*r_exes)[exec]->max << " "; 
    ofl << std::right << std::setw(w_mean) << (*r_exes)[exec]->mean << " "; 
    ofl << std::right << std::setw(w_stdev) << (*r_exes)[exec]->stdev;
    ofl << " "; 
    ofl << std::right << std::scientific << (*r_exes)[exec]->cv << " "; 
    // Samples:
    for (uint32_t thr = 0; thr < nr_threads; thr++) {
      bool reset_color = false;
      if (use_color && col.includes(run, thr)) {
        // Highlight all threads included in the sample. This can be fewer
        // than |msk| in case of scaling runs.
        ofl << C_BOLD_GREEN; 
        reset_color = true;
      }
      ofl << std::right << std::setw(width[thr]);
      ofl << sample[run_offset + exec * nr_threads + thr] << " ";
      if (reset_color) {
        ofl << C_RESET;
      }
    }
    ofl << std::endl;
  }
  printSep(ofl, line_width);
  //
  // Print output over all executions:
  //
  int w_median = std::max(6, numWidth(r_run->median));
  int w_ostdev = std::max(5, numWidth(r_run->stdev));
  ofl << "OVERALL";
  if (::nr_runs > 1) {
    ofl << " EXECUTIONS OF RUN " << run << ":";
  }
  ofl << std::endl;
  ofl << std::setw(w_median) << "MEDIAN" << " ";
  ofl << std::setw(w_ostdev) << "STDEV" << " CV\n";
  printSep(ofl, line_width);
  ofl << std::right << std::setw(w_median) << r_run->median << " ";
  ofl << std::right << std::setw(w_ostdev) << r_run->stdev << " ";
  ofl << std::right << r_run->cv <<std::endl;
  printSep(ofl, line_width); 
}

void dump_all_runs(std::ostream& ofl) {
  int w_run = 0;
  int w_max = 0;
  int w_median = 0;
  int w_mean = 0;
  int w_stdev = 0;
  int w_cv = 0;
  for (uint32_t run = 0; run < nr_runs; run++) {
    w_max = std::max(w_max, numWidth(res_run[run]->max));
    w_median = std::max(w_median, numWidth(res_run[run]->median));
    w_mean = std::max(w_mean, numWidth(res_run[run]->mean));
    w_stdev = std::max(w_stdev, numWidth(res_run[run]->stdev));
    w_cv = std::max(w_cv, numWidth(res_run[run]->cv));
  }
  w_run  = std::max(3, numWidth(nr_runs)); // "RUN" 
  w_max  = std::max(3, w_max); // "MAX"
  w_median = std::max(6, w_max); // "MEDIAN"
  w_mean = std::max(4, w_mean); // "MEAN"
  w_stdev = std::max(5, w_stdev); // "STDEV"
  w_cv = 9;
  int line_width = w_run + w_max + w_median + w_mean + w_stdev + w_cv + 5;
  printSep(ofl, line_width, '@');
  ofl << "Overview across runs:\n";
  printSep(ofl, line_width, '-');
  ofl << std::setw(w_run) << "RUN" << " ";
  ofl << std::setw(w_median) << "MEDIAN" << " ";
  ofl << std::setw(w_max) << "MAX" << " ";
  ofl << std::setw(w_mean) << "MEAN" << " ";
  ofl << std::setw(w_stdev) << "STDEV" << " ";
  ofl << std::setw(w_cv) << "CV" << std::endl;
  printSep(ofl, line_width, '-');
  for (uint32_t run = 0; run < nr_runs; run++) {
    ofl << std::right << std::setw(w_run) << run << " "; 
    ofl << std::right << std::setw(w_median) << res_run[run]->median;
    ofl << " "; 
    ofl << std::right << std::setw(w_max) << res_run[run]->max << " "; 
    ofl << std::right << std::setw(w_mean) << res_run[run]->mean << " "; 
    ofl << std::right << std::setw(w_stdev) << res_run[run]->stdev << " "; 
    ofl << std::right << std::setw(w_cv) << res_run[run]->cv << std::endl; 
  }
  printSep(ofl, line_width, '@');
}

void dumpData(bool ovw_only, smpl_collector * col, const char * f_name) {
  smpl_collector * my_col = col;
  if (col == nullptr) {
    my_col = new range_smpl_collector(nr_threads, 0, nr_threads, ::nr_runs > 1);
  }
  //
  // Collect statistics:
  //
  aggregateStatistics(*my_col);
  //
  // Dump data:
  //
  bool to_file = false;
  std::ofstream ofl;
  if (f_name != nullptr) {
    to_file = true;
    ofl.open(f_name);
  }

  // File-name header on std-out iff file-name is provided:
  if (nullptr != f_name) {
    int h_width = strlen(f_name) + 4;
    bool reset_color = false;
    if (isTerminalStream(std::cout) && supportsColor()) {
      std::cout << C_BOLD_BLUE;
      reset_color = true;
    }
    printSep(std::cout, h_width, 'x');
    std::cout << "x " << f_name << " x\n";
    printSep(std::cout, h_width, 'x');
    if (reset_color) {
      std::cout << C_RESET;
    }
  }

  if (!ovw_only) {
    // Dump executions per run:
    for (uint32_t run = 0; run < ::nr_runs; run++) {
      dump_details_of_run(run, *my_col, std::cout);
      if (to_file) {
        dump_details_of_run(run, *my_col, ofl);
      }
    }
  }

  // Overview over all runs:
  if (ovw_only || nr_runs > 1) {
    dump_all_runs(std::cout);
    if (to_file) {
      dump_all_runs(ofl);
    }
  }

  // De-allocate heap-memory:
  deleteDump(); 

  // Close output file:
  if (to_file) {
    ofl.close();
  }
}

int numWidth(double num) {
  // Note: undefined for NaN and INF
  if (num == 0.0) {
    return 1;
  }
  if (num < 0) {
     num = -num;
  }
  return 1 + (int) floor(log10(num));
}

void printSep(std::ostream& ofl, int cols, char sep) {
  for (int i = 0; i<cols; i++) {
    ofl << sep;
  }
  ofl << std::endl;
}

void deleteDump() {
  for (unsigned int i = 0; i<res_run.size(); i++) {
    delete res_run[i];  
  }
  res_run.clear();
  for (auto i = res_exec.begin(), i_end = res_exec.end(); i != i_end; ++i) {
    for (auto j = (*i)->begin(), j_end = (*i)->end(); j != j_end; ++j) {
      delete *j;
    }
    (*i)->clear();
    delete *i;
  }
  res_exec.clear();
}

void deleteSamples() {
  if (sample != nullptr) {
    free(sample);
  }
}

void dumpRawData(const char * f_name) {
  const char * my_f_name = f_name == nullptr?"dump.txt":f_name;
  std::ofstream ofl;
  // Compute maximum width of each column across all runs:
  int width[nr_threads];
  for (uint32_t run = 0; run < nr_runs; run++) {
    computeSampleWidths(width, run, run==0?true:false);
  }
  ofl.open(my_f_name);
  // Dump all samples:
  for (uint32_t run = 0; run < nr_runs; run++) {
    ofl << "# RUN: " << run << std::endl;
    uint32_t run_offset = nr_executions * nr_threads * run;
    for (uint32_t exec = 0; exec < nr_executions; exec++) {
      ofl << "# EXE: " << exec << std::endl;
      uint32_t exec_offset = run_offset + exec * nr_threads;
      for (uint32_t tr = 0; tr < nr_threads; tr++) {
        ofl << std::setw(width[tr]);
        ofl << sample[exec_offset + tr];
        if (tr+1 == nr_threads) {
          ofl << std::endl;
        } else {
          ofl << " ";
        }
      }
    }
  }
  ofl.close();
}

void computeSampleWidths(int * width, int run, bool initialize) {
  // Initialization:
  if (initialize) {
    for (uint32_t thr = 0; thr < nr_threads; thr++) {
      width[thr] = 0;
    }
  }
  // Compute max width of each column of samples:
  const int run_offset = run * nr_executions * nr_threads;
  for (uint32_t exec = 0; exec < nr_executions; exec++) {
    for (uint32_t thr = 0; thr < nr_threads; thr++) {
      long long s = sample[run_offset + exec * nr_threads + thr];
      width[thr] = std::max(width[thr], numWidth(s));
    }
  }
}
