#include<stdatomic.h>
#include<stdint.h>

int gcc_memorder(int mo)
{
   int memorder;
   switch(mo){
      case 0: memorder = __ATOMIC_RELAXED; break;
      case 1: memorder = __ATOMIC_CONSUME; break;
      case 2: memorder = __ATOMIC_ACQUIRE; break;
      case 3: memorder = __ATOMIC_RELEASE; break;
      case 4: memorder = __ATOMIC_ACQ_REL; break;
      case 5: memorder = __ATOMIC_SEQ_CST; break;
      default: memorder = -1;
   }
   return memorder;
}

void gcc_atomic_load(void* ptr, void* ret, int memorder, int size)
{
   switch(size){
      case 1:
         __atomic_load((uint8_t*)ptr, (uint8_t*)ret, gcc_memorder(memorder));
         break;
      case 2:
         __atomic_load((uint16_t*)ptr, (uint16_t*)ret, gcc_memorder(memorder));
         break;
      case 4:
         __atomic_load((uint32_t*)ptr, (uint32_t*)ret, gcc_memorder(memorder));
         break;
      case 8:
         __atomic_load((uint64_t*)ptr, (uint64_t*)ret, gcc_memorder(memorder));
         break;
#if defined USE_DWATOMIC
      case 16:
         __atomic_load((__uint128_t*)ptr, (__uint128_t*)ret, gcc_memorder(memorder));
         break;
#endif
      default:
         return;
   }
}

void gcc_atomic_store(void* ptr, void* val, int memorder, int size)
{
   switch(size){
      case 1:
         __atomic_store((uint8_t*)ptr, (uint8_t*)val, gcc_memorder(memorder));
         break;
      case 2:
         __atomic_store((uint16_t*)ptr, (uint16_t*)val, gcc_memorder(memorder));
         break;
      case 4:
         __atomic_store((uint32_t*)ptr, (uint32_t*)val, gcc_memorder(memorder));
         break;
      case 8:
         __atomic_store((uint64_t*)ptr, (uint64_t*)val, gcc_memorder(memorder));
         break;
#if defined USE_DWATOMIC
      case 16:
         __atomic_store((__uint128_t*)ptr, (__uint128_t*)val, gcc_memorder(memorder));
         break;
#endif
      default:
         return;
   }
}

int gcc_atomic_compare_exchange(void* ptr, void* expected, void* desired, int size, int weak, int succ, int fail)
{
   switch(size){
      case 1:
         return __atomic_compare_exchange_1((uint8_t*)ptr, (uint8_t*)expected, *((uint8_t*)desired), weak, succ, fail);
      case 2:
         return __atomic_compare_exchange_2((uint16_t*)ptr, (uint16_t*)expected, *((uint16_t*)desired), weak, succ, fail);
      case 4:
         return __atomic_compare_exchange_4((uint32_t*)ptr, (uint32_t*)expected, *((uint32_t*)desired), weak, succ, fail);
      case 8:
         return __atomic_compare_exchange_8((uint64_t*)ptr, (uint64_t*)expected, *((uint64_t*)desired), weak, succ, fail);
#if defined USE_DWATOMIC
      case 16:
         return __atomic_compare_exchange_16((__uint128_t*)ptr, (__uint128_t*)expected, *((__uint128_t*)desired), weak, succ, fail);
#endif
      default:
         return 0;
   }
}
