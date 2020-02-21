#ifndef __FENCES_AARCH64_H__
#define __FENCES_AARCH64_H__ // prevent against double-inclusion

inline void mem_fence(enum fences f) {
  switch(f) {
    case fences::ss:
      asm volatile("dmb ishst" ::: "memory");
      break;
    case fences::ll:
      asm volatile("dmb ishld" ::: "memory");
      break;
    case fences::ll_ss:
      asm volatile("dmb ishld" ::: "memory");
      asm volatile("dmb ishst" ::: "memory");
      break;
    case fences::ls:
      asm volatile("dmb ishld" ::: "memory");
      break;
    case fences::ll_ls:
      asm volatile("dmb ishld" ::: "memory");
      break;
    case fences::ll_ls_ss:
      asm volatile("dmb ishld" ::: "memory");
      asm volatile("dmb ishst" ::: "memory");
      break;
    default:
      asm volatile("dmb ish" ::: "memory");
  }
}

#endif
