#ifndef __FILTER_LOCK_H__
#define __FILTER_LOCK_H__

using delay_func = void(*)(unsigned long);

int higher_or_equal(int, int, int);

void init_filter_lock(int thread_number);
void delete_filter_lock();
void reset_filter_lock(int tid);

void lock(int tid, int thread_number);
void unlock(int tid);
void filter_lock(int tid, int thread_number, int iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

#endif
