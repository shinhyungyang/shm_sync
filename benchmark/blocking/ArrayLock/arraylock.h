#ifndef __ARRAYLOCK_H__
#define __ARRAYLOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void arraylock_sync(int tid, int nr_threads, int nr_iter, 
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_arraylock(int nr_threads);

void delete_arraylock();

#endif
