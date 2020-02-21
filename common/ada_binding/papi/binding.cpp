#include<stdio.h>
#include<string.h>
#include<papi.h>
#include"globals.h"

struct alignas(CACHELINE_SIZE) time_measure {
  long long start;
  long long end;
  long long elapsed;
};

struct time_measure thread_time[MAX_NR_THREADS];

extern "C" {
   void PAPI_setup(void) {
      int retval = PAPI_library_init(PAPI_VER_CURRENT);

      if(retval != PAPI_VER_CURRENT) { 
         printf("PAPI library init error\n");
         return;
      }
   }

   void PAPI_start_tsx(int *eventSet) {
      int codes[3];
      char cpu_events[3][50] = {
         "CPU_CLK_UNHALTED",
         "CPU_CLK_UNHALTED:intx=1",
         "CPU_CLK_UNHALTED:intx=1:intxcp=1"
      };
      PAPI_option_t opt;

      *eventSet = PAPI_NULL;

      if(PAPI_create_eventset(eventSet) != PAPI_OK) {
         printf("PAPI library init error\n");
         return;
      }

      if(PAPI_assign_eventset_component(*eventSet, 0) != PAPI_OK) {
         printf("PAPI_assign_eventset_component error\n");
         return;
      }
      memset(&opt, 0, sizeof (PAPI_option_t));
      opt.inherit.inherit = PAPI_INHERIT_ALL;
      opt.inherit.eventset = *eventSet;
      if(PAPI_set_opt(PAPI_INHERIT, &opt) != PAPI_OK) {
         printf("PAPI_set_opt error\n");
         return;
      }

      if(PAPI_event_name_to_code(cpu_events[0], &codes[0]) != PAPI_OK) {
         printf("PAPI coding error: '%s'\n", cpu_events[0]);
         return;
      }
      if(PAPI_event_name_to_code(cpu_events[1], &codes[1]) != PAPI_OK) {
         printf("PAPI coding error: '%s'\n", cpu_events[1]);
         return;
      }
      if(PAPI_event_name_to_code(cpu_events[2], &codes[2]) != PAPI_OK) {
         printf("PAPI coding error: '%s'\n", cpu_events[2]);
         return;
      }
      if(PAPI_add_events(*eventSet, codes, 3) != PAPI_OK) {
         printf("PAPI add_events error\n");
         return;
      }
      if(PAPI_start(*eventSet) != PAPI_OK) {
         printf("PAPI start error\n");
         return;
      }
   }

   void PAPI_end_tsx(int *eventSet) {
      long long values[3];

      if(PAPI_read(*eventSet, values) != PAPI_OK){
         printf("PAPI read error\n");
         return;
      }
      if(PAPI_stop(*eventSet, values) != PAPI_OK){
         printf("PAPI stop error\n");
         return;
      }

      printf("Total cycles        : %llu\n", values[0]);
      printf("Total TSX cycles    : %llu\n", values[1]);
      printf("Commited TSX cycles : %llu\n", values[2]);
      printf("Succ%%:  %lf\n", (double)values[2]*100/values[1]);
   }

   void PAPI_start_nsec(int tid) {
      thread_time[tid].start = PAPI_get_real_nsec();
   }

   void PAPI_end_nsec(int tid) {
      thread_time[tid].end = PAPI_get_real_nsec();
   }

   long long PAPI_get_nsec(int tid) {
      return thread_time[tid].end - thread_time[tid].start;
   }

   long long PAPI_measure(void) {
      thread_time[0].elapsed = thread_time[0].end - thread_time[0].start;

      long long elapsed_nsec = thread_time[0].elapsed;

      for(int i=1; i != MAX_NR_THREADS; ++i){  
         thread_time[i].elapsed = thread_time[i].end - thread_time[i].start;
         //printf("thread_time[%d].elapsed = %lld\n", i, thread_time[i].elapsed);
         if(thread_time[i].elapsed >= elapsed_nsec)  elapsed_nsec = thread_time[i].elapsed;
      }
      return elapsed_nsec;
   }
}
