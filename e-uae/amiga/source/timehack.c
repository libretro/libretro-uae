/*
 * synchronize system time with the host's time
 *
 * Compile with DCC (or SAS/C) or GCC
 *
 * Copyright 1997, 1999 Bernd Schmidt
 * Copyright 1999 Patrick Ohly
 * Copyright 2003 Richard Drummond
 * 
 * 0.3 - 20031106
 *   Silly me. Timehack was demanding clock be exactly in sync
 *   to 1us accuracy. Make it less strict and only update amiga
 *   time if it's out by more than 1ms.
 * 0.2 - 20031011
 *   Oops. I got the the sign of the offset wrong in TZ. Apparently
 *   negative values are ahead of UTC. Go figure . . .
 * 0.1 - 20031011
 *   Quick and dirty support for using TZ offset added.
 *   Can now be killed with ^C
 *   Can now be built with GCC
 *   Replaced fprintf() with fputs() for smaller code size with GCC.
 */

#include <stdio.h>
#include <stdlib.h>

#include <exec/devices.h>
#include <exec/interrupts.h>
#include <exec/nodes.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <intuition/intuitionbase.h>
#include <intuition/preferences.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <devices/timer.h>
#include <hardware/intbits.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/timer_protos.h>

#ifdef __GNUC__
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <inline/alib.h>
#endif


#ifdef _DCC
int (*calltrap)(__d0 int, __a0 struct timeval *) = 0xF0FFA0;
#elif defined (__GNUC__)
int (*calltrap)(int __asm("d0"),struct timeval * __asm("a0")) = (APTR)0xF0FFA0;
#else
/* SAS */
__asm int (*calltrap)(register __d0 int, register __a0 struct timeval *) = (APTR)0xF0FFA0;
#endif

#ifdef __GNUC__
/* Don't link with command-line parser to save code size */
int __nocommandline = 0;
#endif

/* We'll give this a verson number now. We have to
 * to start somewhere - Rich */
char verstag[] = "\0$VER: timehack 0.3";

#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif


/*
 * get_tz_offset()
 *
 * Get offset from local time to UTC and return it as
 * a timeval (seconds and usecs). The local timezone
 * is read from the env variable TZ and this is expected
 * to be in standard format, for example, EST+5.
 * If TZ isn't present or can't be parsed, this will
 * return an offset 0 seconds.
 */
struct timeval *get_tz_offset()
{
    static struct timeval tz_offset;
    BPTR file;
    char tz_str[7];

    if ((file = Open ("ENV:TZ", MODE_OLDFILE))!=NULL) {
        int len;
        if ((len = Read (file, &tz_str[0], 6)) >= 4) {
            tz_str[len]='\0';
	    // N.B.: negative offsets in ENV:TZ are ahead of UTC
            tz_offset.tv_secs = -1 * atol(&tz_str[3]) * 3600;
        }
        Close(file);
    } else
        tz_offset.tv_secs = 0;

    tz_offset.tv_micro = 0;

    return &tz_offset;
}


int main (int argc, char **argv)
{
    struct timerequest *timereq = 0;
    struct MsgPort *timeport;
    struct timeval *tz_offset;
    struct Device *TimerBase;
    int quit = 0;
    int result = calltrap (0, 0);

    if (result == 1)
	fputs ("timehack already running.\n", stderr);
    else if (result == 2)
	fputs ("timehack not supported with this version of UAE.\n", stderr);
    if (result != 0)
	exit (5);

    timeport = (struct MsgPort *) CreatePort (0, 0);
    if (timeport)
	timereq = (struct timerequest *) CreateStdIO(timeport);

    if (timereq == 0)
	goto fail;

    if (OpenDevice ("timer.device", UNIT_VBLANK, (struct IORequest *) timereq, 0) != 0)
	goto fail;
    TimerBase = timereq->tr_node.io_Device;

    SetTaskPri (FindTask (NULL), 20); /* same as input.device */

    tz_offset = get_tz_offset();

    while (!quit) {
	struct timeval cur_sys_time;

	timereq->tr_node.io_Command = TR_GETSYSTIME;
	DoIO ((struct IORequest *)timereq);
	cur_sys_time = timereq->tr_time;
	calltrap (1, &timereq->tr_time);
	if (timereq->tr_time.tv_secs != cur_sys_time.tv_secs
	    || (timereq->tr_time.tv_secs == cur_sys_time.tv_secs
		&& ABS(timereq->tr_time.tv_micro - cur_sys_time.tv_micro) > 1000))
	{
	    AddTime (&timereq->tr_time, tz_offset);
            timereq->tr_node.io_Command = TR_SETSYSTIME;
            DoIO ((struct IORequest *)timereq);
        }

        timereq->tr_time.tv_secs = 1;
	timereq->tr_time.tv_micro = 0;
        timereq->tr_node.io_Command = TR_ADDREQUEST;
	DoIO ((struct IORequest *)timereq);

        if (SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
	    quit = TRUE;
    }
    /* Exit and error handling badly needs tidying up - Rich */
    CloseDevice ((struct IORequest *)timereq);
    DeleteStdIO ((struct IOStdReq *)timereq);
    DeletePort (timeport);
    exit (0);

    fail:
    fputs ("Couldn't start timehack (that's bad!)\n", stderr);
    exit (5);
}
