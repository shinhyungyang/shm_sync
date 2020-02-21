#include<stdint.h>
#include<stdlib.h>

#define MASK_PTR (uint64_t)0xffffffffffffu //48 bits
#define MASK_CNT ((uint64_t)0xffffu << 48)

uint64_t get_ptr(uint64_t tagged_ptr)
{ return (uint64_t)(tagged_ptr & MASK_PTR); }

void set_ptr(uint64_t *tagged_ptr, uint64_t ptr)
{ *tagged_ptr = (*tagged_ptr & MASK_CNT) | (ptr & MASK_PTR); }

uint64_t get_count(uint64_t tagged_ptr)
{ return tagged_ptr >> 48; }

uint64_t create_tagged_ptr(uint64_t ptr, uint64_t count)
{ return (uint64_t)((ptr & MASK_PTR) | (count << 48)); }
