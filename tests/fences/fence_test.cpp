#include"fences.h"

int foo;

int main() {
  foo = 1;
  mem_fence(fences::ss);
  foo = 2;
  mem_fence(fences::ll);
  foo = 3;
  mem_fence(fences::ll_ss);
  foo = 4;
  mem_fence(fences::ll_ls_ss);
  foo = 5;
  mem_fence(fences::ll_ls_sl_ss);
  return 0;
}
