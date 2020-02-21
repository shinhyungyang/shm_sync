#ifndef __FENCES_H__
#define __FENCES_H__ // prevent against double-inclusion

enum class fences {
  ss          = 0x01,
  sl          = 0x02, 
  ls          = 0x04,
  ll          = 0x08,
  sl_ss       = sl | ss,
  ls_ss       = ls | ss,
  ls_sl       = ls | sl,
  ls_sl_ss    = ls | sl | ss,
  ll_ss       = ll | ss,
  ll_sl       = ll | sl,
  ll_sl_ss    = ll | sl | ss,
  ll_ls       = ll | ls,
  ll_ls_ss    = ll | ls | ss,
  ll_ls_sl    = ll | ls | sl,
  ll_ls_sl_ss = ll | ls | sl | ss  
};

#if defined (__x86_64__)
  #include"fences_x86_64.h"
#elif defined (__aarch64__)
  #include"fences_aarch64.h"
#else
  #error "Unsupported architecture"
#endif

#endif
