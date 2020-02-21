#ifndef __CPP11_LOCK_H__
#define __CPP11_LOCK_H__ // prevent against double-inclusion

using delay_func = void(*)(unsigned long);

void cpp11_mutex_sync(int nr_iter, 
    unsigned long ui, unsigned long uo, delay_func usefu_in, delay_func useful_out);

void init_cpp11_mutex_lock();

void delete_cpp11_mutex_lock();

#endif
