 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Miscellaneous machine dependent support functions and definitions
  *
  * Copyright 1996 Bernd Schmidt
  * Copyright 2004 Richard Drummond
  */

#include <sys/time.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "machdep/m68k.h"
#include "custom.h"
#include "events.h"
#include "sleep.h"
#include "rpt.h"



//struct flag_struct regflags;
             
//static volatile frame_time_t last_time, best_time;
//static volatile int loops_to_go;
void machdep_init (void)
{
    syncbase = 1000000; // 1 MHz 
    //rpt_available = 1;
}

//return time in microseconds (usecond)
frame_time_t read_processor_time (void)
{
     
#ifndef _ANDROID_

#ifdef PS3PORT

#warning "GetTick PS3\n"

	unsigned long	ticks_micro;
	uint64_t secs;
	uint64_t nsecs;

	sys_time_get_current_time(&secs, &nsecs);
	ticks_micro =  secs * 1000000UL + (nsecs / 1000);

	return ticks_micro;
#else
   	struct timeval tv;
   	gettimeofday (&tv, NULL);
   	return tv.tv_sec*1000000 + tv.tv_usec;
#endif

#else

    	struct timespec now;
    	clock_gettime(CLOCK_MONOTONIC, &now);
    	return now.tv_sec*1000000 + now.tv_nsec/1000;
#endif

}

//access all files and directories
int access(const char* path, int mode) {
	return 0;
} 


