/*
 * IEvent mouse hack
 *
 * Compile with DCC
 *
 * Copyright 1997 Bernd Schmidt
 */

#include <stdio.h>

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
#include <clib/exec_protos.h>

int (*calltrap)(__d0 int) = 0xF0FF70;

extern void mousehackint(void);

struct {
    ULONG mx;
    ULONG my;
    ULONG sigbit;
    struct Task *mt;
} foo;

struct Interrupt myint = {
    { 0, 0, NT_INTERRUPT, 5, "UAE mouse hack" },
    &foo,
    mousehackint
};

int main(int argc, char **argv)
{
    struct IntuitionBase *ibase;
    struct InputEvent *iev;
    struct IOStdReq *ioreq = 0;
    struct MsgPort *port;

    struct timerequest *timereq = 0;
    struct MsgPort *timeport;
    int oldx = 0, oldy = 0;
    int i;

    i = (*calltrap) (0);
    if (i == 0) {
	fprintf(stderr, "mousehack not needed for this version of UAE.\n");
	exit (0);
    }
    if (i == -1) {
	fprintf(stderr, "mousehack already running.\n");
	exit (5);
    }

    i = AllocSignal (-1);
    if (i < 0)
	goto fail;
    foo.sigbit = 1 << i;

    port = CreatePort(0, 0);
    timeport = CreatePort (0, 0);
    if (port)
	ioreq = CreateStdIO(port);

    if (timeport)
	timereq = CreateStdIO(timeport);

    if (ioreq == 0)
	goto fail;
    if (timereq == 0)
	goto fail;

    iev = AllocMem (sizeof (struct InputEvent), MEMF_CLEAR + MEMF_PUBLIC);
    if (iev == 0)
	goto fail;
    if (OpenDevice ("input.device", 0, ioreq, 0) != 0)
	goto fail;
    if (OpenDevice ("timer.device", 0, timereq, 0) != 0)
	goto fail;

    foo.mx = (ULONG)-1;
    foo.my = (ULONG)-1;
    foo.mt = FindTask (0);
    AddIntServer(INTB_VERTB, &myint);

    ibase = OpenLibrary ("intuition.library", 0);
    SetTaskPri (foo.mt, 20); /* same as input.device */
    for (;;) {
	int newx, newy;

	Wait (foo.sigbit);
	ioreq->io_Command = IND_WRITEEVENT;
	ioreq->io_Length = sizeof (struct InputEvent);
	ioreq->io_Data = iev;
	ioreq->io_Flags = IOF_QUICK;
	iev->ie_Class = IECLASS_POINTERPOS;
	iev->ie_SubClass = 0;
	iev->ie_Code = 0;
	iev->ie_Qualifier = 0;
#if 0
	newx = (*calltrap) (1);
	newy = (*calltrap) (2);

	if (oldy != newy || oldx != newx)
#endif
	{
	    timereq->tr_node.io_Flags = IOF_QUICK;
	    timereq->tr_node.io_Command = TR_GETSYSTIME;
	    DoIO (timereq);
	    iev->ie_TimeStamp = timereq->tr_time;
	    /* Those are signed, so I hope negative values are OK... */
	    /* I wonder why I have to multiply those by 2... but it works,
	     * at least for me. */
	    iev->ie_position.ie_xy.ie_x = foo.mx - ibase->ViewLord.DxOffset*2;
	    iev->ie_position.ie_xy.ie_y = foo.my - ibase->ViewLord.DyOffset*2;

	    oldx = newx;
	    oldy = newy;
	    DoIO(ioreq);
	}
#if 0
	timereq->tr_node.io_Flags = IOF_QUICK;
	timereq->tr_time.tv_secs = 0;
	timereq->tr_time.tv_micro = 20000;
	timereq->tr_node.io_Command = TR_ADDREQUEST;
	DoIO(timereq);
#endif
    }
    fail:
    fprintf (stderr, "Couldn't start mousehack (that's bad!)\n");
    exit (5);
}
