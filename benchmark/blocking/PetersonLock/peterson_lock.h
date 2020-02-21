#ifndef __PETERSON_LOCK_H__
#define __PETERSON_LOCK_H__

using delay_func = void(*)(unsigned long);

void reset_peterson_lock();

void lock(int tid);
void unlock(int tid);
void peterson_lock(int tid, int iterM,
    unsigned long ui, unsigned long uo, delay_func useful_in, delay_func useful_out);

#endif
