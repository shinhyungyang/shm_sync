#ifndef __FAIR_LOCK_H__
#define __FAIR_LOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

inline void entry_section(int tid, int nr_threads);

inline void exit_section(int tid);

void fair_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_fair_lock(int nr_threads);

void delete_fair_lock();

#endif
