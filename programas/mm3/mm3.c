#include <papi.h>
#include <stdio.h>
#include <stdlib.h>   // exit()

#define N 1024
#define CLS 1

#define SM (CLS / sizeof (mat_element))

typedef short mat_element;

int handle_error(int code, char *outstring);

mat_element mul1[N][N] __attribute__ ((aligned (sizeof(CLS))));
mat_element mul2[N][N] __attribute__ ((aligned (sizeof(CLS))));
mat_element res[N][N]  __attribute__ ((aligned (sizeof(CLS))));

int main(){

  /************************************/

  mat_element *rmul1, *rmul2, *rres;
  long_long   checksum = 0;
  int i,j,k;
  int i2,j2,k2;

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j){
      mul1[i][j]= (i+j)   % 8 + 1;
      mul2[i][j]= (N-i+j) % 8 + 1;
      res[i][j] = 0;
    }

  /************************************/

  int retval, EventSet=PAPI_NULL;
  long_long values[3];
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

  /* Add L1 data cache misses to the Event Set */
  if (PAPI_add_event(EventSet,PAPI_L1_DCM) != PAPI_OK)
    handle_error(1,"add_event");
  /* Add load instructions completed to the Event Set */
  if (PAPI_add_event(EventSet,PAPI_LD_INS) != PAPI_OK)
    handle_error(1,"add_event");
  /* Add store instructions completed to the Event Set */
  if (PAPI_add_event(EventSet,PAPI_SR_INS) != PAPI_OK)
    handle_error(1,"add_event");
 
  /* Reset the counting events in the Event Set */
  if (PAPI_reset(EventSet) != PAPI_OK)
    handle_error(1,"reset");

  /* Read the counting of events in the Event Set */
  if (PAPI_read(EventSet, values) != PAPI_OK)
    handle_error(1,"read");

  printf("After resetting counter 'PAPI_L1_DCM' [x10^6]: %f\n", \
        (double)(values[0])/1000000);
  printf("After resetting counter 'PAPI_LD_INS' [x10^6]: %f\n", \
	(double)(values[1])/1000000);
  printf("After resetting counter 'PAPI_SR_INS' [x10^6]: %f\n", \
	(double)(values[2])/1000000);

  /* Start counting events in the Event Set */
  if (PAPI_start(EventSet) != PAPI_OK)
    handle_error(1,"start");
 
  /* Gets the starting time in clock cycles */
  start_cycles = PAPI_get_real_cyc();
 
  /* Gets the starting time in microseconds */
  start_usec = PAPI_get_real_usec();

  /************************************/
  /*      MATRIX MULTIPLICATION       */
  /************************************/

  for (i = 0; i < N; i += SM)
    for (j = 0; j < N; j += SM)
      for (k = 0; k < N; k += SM)
        for (i2 = 0, rres = &res[i][j], rmul1 = &mul1[i][k]; i2 < SM; ++i2, rres += N, rmul1 += N)
          for (k2 = 0, rmul2 = &mul2[k][j]; k2 < SM; ++k2, rmul2 += N)
            for (j2 = 0; j2 < SM; ++j2)
              rres[j2] += rmul1[k2] * rmul2[j2];

  /************************************/

  /* Gets the ending time in clock cycles */
  end_cycles = PAPI_get_real_cyc();
 
  /* Gets the ending time in microseconds */
  end_usec = PAPI_get_real_usec();

  /* Stop the counting of events in the Event Set */
  if (PAPI_stop(EventSet, values) != PAPI_OK)
    handle_error(1,"stop");

  printf("After stopping counter 'PAPI_L1_DCM'  [x10^6]: %f\n", \
	 (double)(values[0])/1000000);
  printf("After stopping counter 'PAPI_LD_INS'  [x10^6]: %f\n", \
	 (double)(values[1])/1000000);
  printf("After stopping counter 'PAPI_SR_INS'  [x10^6]: %f\n", \
	 (double)(values[2])/1000000);

  printf("Wall clock cycles [x10^6]: %f\n",           \
        (double)(end_cycles - start_cycles)/1000000);
  printf("Wall clock time [seconds]: %f\n",           \
        (double)(end_usec - start_usec)/1000000); 

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      checksum+=res[i][j];
  printf("Matrix checksum: %lld\n", checksum); 

  return(0);
}
 

int handle_error(int code, char *outstring){
  char errmesg[PAPI_MAX_STR_LEN];

  printf("Error in PAPI function call %s\n", outstring);
  PAPI_perror("PAPI Error");
  exit(1);
}
