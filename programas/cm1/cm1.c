#include <papi.h>
#include <stdio.h>
#include <stdlib.h>   // exit()

#define N (1024*1024)
#define CACHE_MIN (8*1024)
#define CACHE_MAX (64*1024)

#define LOGFILE "cm1.out"

typedef char data_cell;

int handle_error(int code, char *outstring);

main(){

  /************************************/

  data_cell A[N];

  register int csize;
  register int stride;
  register int limit;
  register int repeat;
  register int index;
  register int counter;
  float avgMISSES;
  float avgTIME;
  FILE* logfile;

  /************************************/

  int retval, EventSet=PAPI_NULL;
  long_long values[2];
  long_long start_cycles, end_cycles, start_usec, end_usec;

  /* Initialize the PAPI library */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
 
  /* Create the Event Set */
  if (PAPI_create_eventset(&EventSet) != PAPI_OK)
    handle_error(1, "create_eventset");
 
  /* Add L1 data cache misses to our Event Set */
  if (PAPI_add_event(EventSet, PAPI_L1_DCM) != PAPI_OK)
    handle_error(1,"add_event");

  /************************************/

  logfile=fopen(LOGFILE,"w");

  for(csize=CACHE_MIN; csize <= CACHE_MAX; csize=csize*2)
    for(stride=1; stride <= csize/2; stride=stride*2){ 
      limit=csize-stride+1;
 
      /* Reset the counting events in the Event Set */
      if (PAPI_reset(EventSet) != PAPI_OK)
        handle_error(1,"reset");

      /* Read the counting of events in the Event Set */
      if (PAPI_read(EventSet, values) != PAPI_OK)
        handle_error(1,"read");

      /* Start counting events in the Event Set */
      if (PAPI_start(EventSet) != PAPI_OK)
        handle_error(1,"start");
 
      /* Gets the starting time in clock cycles */
      start_cycles = PAPI_get_real_cyc();
 
      /* Gets the starting time in microseconds */
      start_usec = PAPI_get_real_usec();

      /************************************/

      for(repeat=0,counter=0; repeat<=200*stride; repeat++)
        for(index=0; index<limit; index+=stride,counter++)
          A[index] = A[index] + 1;
 
      /************************************/

      /* Gets the ending time in clock cycles */
      end_cycles = PAPI_get_real_cyc();
 
      /* Gets the ending time in microseconds */
      end_usec = PAPI_get_real_usec();

      /* Stop the counting of events in the Event Set */
      if (PAPI_stop(EventSet, values) != PAPI_OK)
        handle_error(1,"stop");

      avgMISSES=(float)(values[0])/counter;
      avgTIME=(float)(end_usec - start_usec)/counter;
      printf("CSIZE=%d \tSTRIDE=%d \tavgMISSES=%f \tavgTIME=%f\n", csize,stride,avgMISSES,avgTIME); 
      fprintf(logfile,"CSIZE=%d \tSTRIDE=%d \tavgMISSES=%f \tavgTIME=%f\n", csize,stride,avgMISSES,avgTIME); 
    }
  fclose(logfile);
}

 
int handle_error(int code, char *outstring)
{
  printf("Error in PAPI function call %s\n", outstring);
  PAPI_perror("PAPI Error");
  exit(1);
}
