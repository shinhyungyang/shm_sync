#ifndef __TASLOCK_H__
#define __TASLOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void taslock_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_taslock();

void delete_taslock();

#endif
