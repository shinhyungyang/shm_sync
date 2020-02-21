#ifndef __FENCES_X86_64_H__
#define __FENCES_X86_64_H__ // prevent against double-inclusion

inline void mem_fence(enum fences f) {
  switch(f) {
    case fences::ss:
      asm volatile("sfence" ::: "memory");
      break;
    case fences::ll:
      asm volatile("lfence" ::: "memory");
      break;
    case fences::ll_ss:
      asm volatile("lfence" ::: "memory");
      asm volatile("sfence" ::: "memory");
      break;
    default:
      asm volatile("mfence" ::: "memory");
  }
}

#endif
