#ifndef __POSIX_LOCK_H__
#define __POSIX_LOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void posix_sync(int tid, int nr_threads, int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_posix();

void delete_posix();

#endif
