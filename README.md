# Introduction

The goal of this assignment is twofold: to determine the characteristics of a computer’s caches, and to leverage the obtained knowledge about the caches in order to optimize the performance of a given
program. For this task, the students will make use of a performance analysis tool that have direct access
to hardware performance counters available on most modern microprocessors. The tool that will be used
is the standard Application Programming Interface (API): PAPI.

In the rest of this section, we make a brief introduction to PAPI, and describe the targeted computer
platform and the development environment. We also describe the procedure for modeling the L1
and L2 caches of the targeted platform, and provide a guide for analyzing the performance
of a matrix-multiply code segment and optimizing it based on the characteristics of the L2 cache of the
target architecturn.

## PAPI - Performance Application Programming Interface

The PAPI project specifies a standard Application Programming Interface (API) for accessing hardware
performance counters available on most modern microprocessors. These counters exist as a small
set of registers that count Events, defined as occurrences of specific signals related to the processor’s
function (such as cache misses and floating point operations), while the program executes on the processor.

Monitoring these events may have a variety of uses in application performance analysis and tuning,
since it facilitates the correlation between the source/object code structure and the efficiency of the actual
mapping of such code in the underlying architecture. Besides performance analysis, including hand
tuning, this set of information may also be used in compiler optimization, debugging, benchmarking,
monitoring and performance modeling.

PAPI has been implemented on a number of different platforms, including: Alpha; MIPS R10K and
R12K; AMD Athlon and Opteron; Intel Pentium II, Pentium III, Pentium M, Pentium IV, Itanium 1 and
Itanium 2; IBM Power 3, 4 and 5; Cell; Sun UltraSparc I, II and II, etc.

Although each processor has a number of events that are native to that specific architecture, PAPI
provides a software abstraction of these architecture-dependent Native Events into a collection of Preset
Events, also known as predefined events, that define a common set of events deemed relevant and useful
for application performance tuning. These events are typically found in many CPUs that provide performance
counters. They give access to the memory hierarchy, cache coherence protocol events, cycle
and instruction counts, functional unit, and pipeline status. Hence, preset events may be regarded as
mappings from symbolic names (`PAPI` preset name) to machine specific definitions (native countable
events) for a particular hardware resource. For example, Total Cycles (in user mode) is mapped into
`PAPI_TOT_CYC`. Some presets are derived from the underlying hardware metrics. For example, Total
L1 Cache Misses (`PAPI_L1_TCM`) is the sum of L1 Data Misses and L1 Instruction Misses on a given
platform. The list of preset and native events that are available on a specific platform can be obtained
by running the commands `papi_avail` and `papi_native_avail`, both provided by the papi source
distribution.

Besides the standard set of events for application performance tuning, PAPI specification also includes
both a high-level and a low-level sets of routines for accessing the counters. The high level
interface consists of eight functions that make it easy to get started with PAPI, by simply providing the
ability to start, stop, and read sets of events. This interface is intended for the acquisition of simple but
accurate measurement by application engineers:

• `PAPI_num_counters` – get the number of hardware counters available on the system;
• `PAPI_flips` – simplified call to get Mflips/s (floating point instruction rate), real and processor time;
• `PAPI_flops` – simplified call to get Mflops/s (floating point operation rate), real and processor time;
• `PAPI_ipc` – gets instructions per cycle, real and processor time;
• `PAPI_accum_counters` – add current counts to array and reset counters;
• `PAPI_read_counters` – copy current counts to array and reset counters;
• `PAPI_start_counters` – start counting hardware events;
• `PAPI_stop_counters` – stop counters and return current counts.

The following is a simple code example of using the high-level API:

```
#include <papi.h>
#define NUM_FLOPS 10000
#define NUM_EVENTS 1

main()
{
  int Events[NUM_EVENTS] = {PAPI_TOT_INS};
  long_long values[NUM_EVENTS];
  
  /* Start counting events */
  if (PAPI_start_counters(Events, NUM_EVENTS) != PAPI_OK)
    handle_error(1);
    
  do_some_work();
  
  /* Read the counters */
  if (PAPI_read_counters(values, NUM_EVENTS) != PAPI_OK)
    handle_error(1);
    
  printf("After reading the counters: %lld\n",values[0]);
  do_some_work();
  
  /* Add the counters */
  if (PAPI_accum_counters(values, NUM_EVENTS) != PAPI_OK)
    handle_error(1);
    
  printf("After adding the counters: %lld\n", values[0]);
  do_some_work();
  
  /* Stop counting events */
  if (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK)
    handle_error(1);
    
  printf("After stopping the counters: %lld\n", values[0]);
}
```

Possible output:

```
After reading the counters: 441027
After adding the counters: 891959
After stopping the counters: 443994
```

The fully programmable low-level interface provides more sophisticated options for controlling the
counters, such as setting thresholds for interrupt on overflow, as well as access to all native counting
modes and events. Such interface is intended for third-party tool writers or users with more sophisticated
needs.

PAPI specification also provides the access to the most accurate timers available on the platform in
use. These timers can be used to obtain both real and virtual time on each supported platform: the real
time clock runs all the time (e.g. a wall clock), while the virtual time clock runs only when the processor
is running in user mode.

In the following code example, `PAPI_get_real_cyc()` and `PAPI_get_real_usec()` are used to
obtain the real time it takes to create an event set in clock cycles and in microseconds, respectively:

```
#include <papi.h>
main()
{
  long_long start_cycles, end_cycles, start_usec, end_usec;
  int EventSet = PAPI_NULL;
  
  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    exit(1);
    
  /* Gets the starting time in clock cycles */
  start_cycles = PAPI_get_real_cyc();
  
  /* Gets the starting time in microseconds */
  start_usec = PAPI_get_real_usec();
  
  /*Create an EventSet */
  if (PAPI_create_eventset(&EventSet) != PAPI_OK)
    exit(1);
  do_some_work();
  
  /* Gets the ending time in clock cycles */
  end_cycles = PAPI_get_real_cyc();
  
  /* Gets the ending time in microseconds */
  end_usec = PAPI_get_real_usec();
  
  printf("Wall clock cycles: %lld\n", end_cycles - start_cycles);
  prinf(“Wall clock time in microseconds: %lld\n”, end_usec - start_usec);
}
```

Possible output:
```
Wall clock cycles: 100173
Wall clock time in microseconds: 136
```

## Targeted Platform and Development Environment

This assignment must be performed on the computers of the lab classes. These computers have similar
hardware characteristics, and any of them can be used as a target platform. Note that, since this work is
hardware-dependent, conducting it on an computer with different hardware characteristics could produce
unexpected results.

To properly setup the development environment, it is necessary to obtain the PAPI library and a set
of auxiliary program files. This material can be found in the package `lab3_kit.zip`, which can be
downloaded from the course website. After downloading and uncompressing this package on any of
the lab classes’ computers, PAPI must be built. To this end, change directories to the location of the
PAPI source code: folder `papi/`. Compile the code by issuing the commands: `./configure`, and
make. This operation will produce a set of helper tools located in directory `src/utils/` and create the
PAPI library `papilib.a`. The tool `papi_avail`, in particular, is useful to determine the PAPI events
supported on the target platform. The library will be linked to the auxiliary programs presented in the
following sections.




# Procedure

## Modeling Computer Caches

In the first part of this assignment, the goal is to model the characteristics of the L1 data cache and L2
cache of the targeted computer platform. Next, we provide instructions for performing this analysis.

### Modeling the L1 Data Cache

The methodology to experimentally model the L1 data cache consists of considering the total amount of
data cache misses during the execution of the following code sequence of program cm1.c. This program can be found in the package `lab3_kit.zip`.

```
for(csize=CACHE_MIN; csize < CACHE_MAX; csize=csize*2)
  for(stride=1; stride <= csize/2; stride=stride*2)
  {
    limit = csize - stride + 1;
    for(repeat=0; repeat<=200*stride; repeat++)
      for(index=0; index<limit; index+=stride)
        x[index] = x[index] + 1;
  }
```

The meaning of each variable is the following:

`x[]` – an arbitrary large array (1Mega-entries) that will be repeatedly accessed to measure the cache
miss pattern;

`csize` – value of the cache size under test; all cache sizes given by integer powers of 2, between
`CACHE_MIN = 8kB` and `CACHE_MAX = 64kB` should be considered;

`stride` – states how many entries are being skipped at each access; for example, if the stride is 4,
entries 0, 4, 8, 12, ... in the array are being accessed, while entries 1, 2, 3, 5, 6, 7, 9, 10, 11,
... are skipped;

`limit` – the greatest address that will be accessed for the cache size and access pattern under test;

`repeat` – denotes the number of times that each access pattern will be repeated in array `x[]`.

Compile the program `cm1.c` using the provided `Makefil`e and execute `cm1`.

Plot the variation of the average number of misses (Avg Misses) with the stride size, for each considered dimension of the L1 data cache (8kB, 16kB, 32kB and 64kB).

NOTE: A fast sketch of these plots can be drawn in your computer by running the following commands:

```
./cm1 > cm1.out
./cm1_proc.sh
```
