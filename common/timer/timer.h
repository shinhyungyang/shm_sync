#ifndef __TIMER_H__
#define __TIMER_H__

extern "C" {
void useful_work_us(unsigned long usecs);

void useful_work_ns(unsigned long nsecs);

void __const_udelay(unsigned long xloops) __attribute__((noinline));

void calibrate_delay();
}

#endif
