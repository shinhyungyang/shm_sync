#ifndef __GLOBALS_H__
#define __GLOBALS_H__ // prevent against double-inclusion

#define CACHELINE_SIZE  64

#define MAX_ALLOC_NR_THREADS 4096
// Maximum number of threads that the statically allocated data-structures
// support.

#define MAX_NR_THREADS 28
// Maximum number of threads for an experimental run. Allowed to be
// over-ridden on the command-line. Thus a benchmark which allows to
// over-ride this value must check that the new value is <=
// MAX_ALLOC_NR_THREADS. 

static_assert (MAX_NR_THREADS <= MAX_ALLOC_NR_THREADS,
               "MAX_NR_THREADS exceeds compile-time maximum"); 
#endif

