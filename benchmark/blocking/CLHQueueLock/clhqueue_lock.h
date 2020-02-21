#ifndef __CLHQUEUE_LOCK_H__
#define __CLHQUEUE_LOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void clhqueue_sync(int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_clhqueue_lock();

void delete_clhqueue_lock();

void init_per_exec_global();

void init_per_exec_per_thread();

void delete_per_thread();

#endif
