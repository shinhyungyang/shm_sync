#ifndef __STATS_H__
#define __STATS_H__ // prevent against double-inclusion

#include"smpl_collector.h"

extern "C" {
  void initStats(uint32_t nr_threads, uint32_t nr_exec, uint32_t nr_runs);
  // Initialize data-structure for the storage of profile-data.

  void deleteSamples();
  // Free data-structure for storage of profile data.

  void enterSample(long long s, uint32_t tid, uint32_t nr_exec, uint32_t run);
  // Enter one sample into the profile-data.

  void dumpData(bool ovw_only, smpl_collector * col = nullptr,
                const char * f_name = nullptr);
  // If f_name is non-NULL, data will be dumped to file f_name in addition
  // to std::cout.
}

#endif
