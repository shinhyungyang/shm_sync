#ifndef __TATAS_LOCK_H__
#define __TATAS_LOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void tatas_sync(int nr_iter,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

void init_tatas_lock();

void delete_tatas_lock();

#endif
