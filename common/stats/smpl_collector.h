#ifndef __SMPL_COLLECTOR_H__
#define __SMPL_COLLECTOR_H__ // prevent against double-inclusion

#include<cstdint>

//
// The purpose of the smpl_collector class hierarchy is to provide a way to
// selectively pick samples from an ensemble of threads. This sample-selection
// can then be used in the stats-component. A typical scenario would be to
// collect the samples of all producer threads, or the samples of all
// consumer-threads.
//
// This class-hierarchy provides classes for the most common cases. Further
// cases can be added by extension.
//

class smpl_collector {
  protected:
    bool is_scaling;
    // True if scaling runs are used.
  public:
    virtual long long * get_samples(uint32_t run, long long * row) = 0;
    // Return ptr to a consecutive memory-area that contains the samples of
    // this run, collected from the sample-row ``row''.

    virtual uint32_t get_nr_samples(uint32_t run) = 0;
    // Return the number of samples included in run ``run''.

    virtual bool includes(uint32_t run, uint32_t sample_nr) = 0;
    // True if ``sample_nr'' is included in ``run''.

    virtual void dumpMask() = 0;
};

class range_smpl_collector: public smpl_collector {
  protected:
    uint32_t population_size;
    uint32_t start_index;
    uint32_t stop_index; // Half open-interval: [start_index, stop_index[
    uint32_t nr_samples;
    long long * sample;
  public:
    range_smpl_collector(uint32_t population_size,
                         uint32_t start_index,
                         uint32_t stop_index,
                         bool is_scaling);
    ~range_smpl_collector();
    long long * get_samples(uint32_t run, long long * row);
    uint32_t get_nr_samples(uint32_t run);
    bool includes(uint32_t run, uint32_t sample_nr);
    void dumpMask();
};

/*
class odd_smpl_collector: public all_smpl_collector {
  public:
    odd_smpl_collector(uint32_t nr_threads, bool is_scaling);
    long long * get_samples(uint32_t run, long long * row);
    bool includes(uint32_t run, uint32_t sample_nr);
    void dumpMask();
};
*/
#endif
