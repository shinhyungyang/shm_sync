#include"stats.h"
#include<string.h>

extern "C" {
  void dumpSingle(bool ovw_only){
     dumpData(ovw_only);
  }
  
  void dumpPair(bool ovw_only, int nr_pairs, char* f_name, bool sc_runs)
  {
     char tmp[256];

     strcpy(tmp, f_name);
     range_smpl_collector col_lower(2 * nr_pairs, 0, nr_pairs, sc_runs);
     dumpData(ovw_only, &col_lower, strcat(tmp, "_producers.txt"));
  
     strcpy(tmp, f_name);
     range_smpl_collector col_upper(2 * nr_pairs, nr_pairs, 2 * nr_pairs, sc_runs);
     dumpData(ovw_only, &col_upper, strcat(tmp, "_consumers.txt"));
  
     deleteSamples();
  }
}
