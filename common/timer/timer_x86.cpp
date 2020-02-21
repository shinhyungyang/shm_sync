#include"timer.h"
#include<sys/times.h>
#include<time.h>
#include<unistd.h>
#define JIFFIES (unsigned long)times(dummy_tms)

unsigned long loops_per_jiffy;

const unsigned long LPS_PREC = 8; // defined by default in init/calibrate.c 

const unsigned long HZ =sysconf(_SC_CLK_TCK); // number of clock ticks per second 

static inline void delay_loop(unsigned long loops)
{
  asm volatile(
      "     test %0, %0 \n"
      "     jz 3f       \n"
      "     jmp 1f      \n"
      ".align 16        \n"
      "1:   jmp 2f      \n"
      ".align 16        \n"
      "2:   dec %0      \n"
      "     jnz 2b      \n"
      "3:   dec %0      \n"
      :
      :"a"(loops)
      );
}

void useful_work_us(unsigned long usecs) {
  __const_udelay(usecs * 0x000010c7); /* 2**32 / 1000000(10^6) (rounded up) */
}

void useful_work_ns(unsigned long nsecs) {
  __const_udelay(nsecs * 0x00005);/* 2**32 / 1000000000(10^9) (rounded up) */
}

void __const_udelay(unsigned long xloops) {
  unsigned long lpj = loops_per_jiffy;
  int d0;
  xloops *= 4;
  
  asm("mull %%edx"
      :"=d" (xloops), "=&a" (d0)
      :"1" (xloops), "0" (lpj * (HZ / 4)));
  
  delay_loop(++xloops);
}

struct tms * dummy_tms = nullptr;

// jiffies == times(dummy_tms)
void calibrate_delay() {
  unsigned long lpj, lpj_base, ticks, loopadd,loopadd_base, chop_limit;
  int trial = 0, band = 0, trial_in_band = 0;
  lpj = (1 << 12);
  ticks = JIFFIES;
  while (ticks == JIFFIES) ;
  ticks = JIFFIES;
  do {
    if (++trial_in_band == (1<<band)) {
      ++band;
      trial_in_band = 0;
    }
    delay_loop(lpj * band); // __delay(lpj * band);
    trial += band;
  } while (ticks == JIFFIES);
  trial -= band;
  loopadd_base = lpj * band;
  lpj_base = lpj * trial;
recalibrate:
  lpj = lpj_base;
  loopadd = loopadd_base;

  chop_limit = lpj >> LPS_PREC;
  while (loopadd > chop_limit) {
    lpj += loopadd;
    ticks = JIFFIES;
    while (ticks == JIFFIES) ;
    ticks = JIFFIES;
    delay_loop(lpj); // __delay(lpj);
    if (JIFFIES != ticks)
      lpj -= loopadd;
    loopadd >>= 1;
  }
  if (lpj + loopadd * 2 == lpj_base + loopadd_base * 2) {
    lpj_base = lpj;
    loopadd_base <<= 2;
    goto recalibrate;
  }
  loops_per_jiffy = lpj;
}
