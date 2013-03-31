 /*
  * UAE - The Un*x Amiga Emulator
  *
  * ami-rexx.c
  *
  * Copyright 1996 Samuel Devulder.
  * Copyright 2004-2006 Richard Drummond
  *
  * History:
  *     05/09/97: Added UAEEXE support.
  *	04/03/98: Added event-hack for SOUND(). Added do_inhibit_frame().
  *	03/05/98: Modified currprefs to changed_prefs where necessary.
  */

/****************************************************************************/

#if defined(POWERUP)
# include <exec/exec.h> /* sam: I've added this or else I have
			 * warnings about missing struct def. */
#ifndef USE_CLIB
#  include <powerup/ppcproto/exec.h>
#  include <powerup/ppcproto/rexxsyslib.h>
#  include <powerup/ppcproto/dos.h>
# else
#  include <clib/exec_protos.h>
#  include <clib/alib_protos.h>
# endif
#else
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/rexxsyslib.h>
# include <proto/dos.h>
# ifndef __SASC
#  include <clib/alib_protos.h>
# endif
#endif

#include <rexx/storage.h>
#include <rexx/errors.h>

/* this prevent a conflict between <rexx/rexxio.h> and <sys/dirent.h> */
#undef DT_DIR

/****************************************************************************/

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "disk.h"
#include "audio.h"
#include "uaeexe.h"
#include "xwin.h"
#include "drawing.h"
#include "debug.h"
#include "version.h"

#include <ctype.h>

/****************************************************************************/

#define UAE_PORTNAME    "UAE"
#define RESULT_LEN      1024
#define PORT_LEN        80
#define CMD_LEN         1024

typedef struct {
    char port[PORT_LEN];
    char cmd_on[CMD_LEN];
    char cmd_off[CMD_LEN];
} gui_rexx_s;

enum {
    LED_POW, LED_DF0, LED_DF1, LED_DF2, LED_DF3,
    NAME_DF0, NAME_DF1, NAME_DF2, NAME_DF3,
    ON_EXIT,
    GUI_REXX_MAX
};

/****************************************************************************/

int  rexx_init (void);
void rexx_exit (void);
void rexx_led (int led, int on);
void rexx_filename (unsigned int num, const char *name);
void rexx_handle_events (void);

/****************************************************************************/

#if defined __GNUC__ && !defined __amigaos4__ && !defined __morphos__
typedef struct RxsLib REXXBASE;
#else
typedef struct Library REXXBASE;
#endif

REXXBASE		*RexxSysBase = NULL;

#ifdef __amigaos4__
struct RexxSysIFace	*IRexxSys;
#endif

static struct MsgPort   *ARexxPort;
static gui_rexx_s        gui_rexx[GUI_REXX_MAX];
static char              RESULT[RESULT_LEN];
static int               led_state[5];

static int   ADDRESS    (const char *hostname, const char *cmd);
static int   matchstr   (const char **line, const char *pat);
static void  extractstr (const char **line, char *result, unsigned int len);
static int   matchnum   (const char **line);

/****************************************************************************/

extern ULONG frame_num;                                  /* ami-win.c */

/****************************************************************************/

int rexx_init (void)
{
    RexxSysBase = (REXXBASE *) OpenLibrary ("rexxsyslib.library", 0L);
    if (!RexxSysBase) {
	write_log ("Can't find rexxsyslib.library!\n");
	return 0;
    }
#ifdef __amigaos4__
    IRexxSys = (struct RexxSysIFace *) GetInterface (RexxSysBase, "main", 1, NULL);
    if (!IRexxSys) {
	CloseLibrary (RexxSysBase);
	RexxSysBase = 0;
	return 0;
    }
#endif

    if (FindPort(UAE_PORTNAME)) {
	write_log ("Port \"%s\" already exists!\n", UAE_PORTNAME);
	return 0;
    }
    ARexxPort = CreatePort (UAE_PORTNAME, 0);
    if (!ARexxPort) {
	write_log ("Failed to open AREXX port \"%s\"!\n", UAE_PORTNAME);
	return 0;
    }
    write_log ("Rexx port \"%s\" installed.\n", UAE_PORTNAME);
    rexx_handle_events ();
    return 1;
}

/****************************************************************************/

void rexx_exit (void)
{
    if (ARexxPort) {
	struct RexxMsg *msg;
	gui_rexx_s *gui = &gui_rexx[ON_EXIT];

	if (gui->port[0] && gui->cmd_on[0]) {
	    if (ADDRESS(gui->port, gui->cmd_on) != RC_OK) {
		write_log ("%s:%s:%s\n", gui->port,
					 gui->cmd_on,
					 RESULT);
	    }
	    gui->port[0] = '\0';
	}
	Forbid ();
	while ((msg = (struct RexxMsg*)GetMsg (ARexxPort))) {
	   msg->rm_Result1 = RC_ERROR;
	   msg->rm_Result2 = 0;
	   ReplyMsg ((void*)msg);
	}
	DeletePort (ARexxPort);
	Permit ();
	ARexxPort = NULL;
    }
#ifdef __amigaos4__
    if (IRexxSys) {
	DropInterface ((struct Interface *)IRexxSys);
	IRexxSys = NULL;
    }
#endif
    if (RexxSysBase) {
	CloseLibrary ((void*)RexxSysBase);
	RexxSysBase = NULL;
    }
}

/****************************************************************************/

static int EJECT (const char *line)
{
    unsigned int drive = matchnum (&line);
    int result = RC_WARN;

    if (drive < 4) {
	if (currprefs.dfxtype[drive] >= 0) {
	    disk_eject (drive);
	    sprintf (RESULT, "Drive %d ejected.", drive);
	    result = RC_OK;
	} else
	    sprintf (RESULT, "Drive %d is disabled.", drive);
    }
    return result;
}

/****************************************************************************/

static int INSERT (const char *line)
{
    unsigned int drive = matchnum (&line);
    int result = RC_WARN;

    if (drive < 4) {
	if (currprefs.dfxtype[drive] >= 0) {
	    char buff[256];
	    extractstr (&line, buff, 256);
	    strcpy (changed_prefs.df[drive], buff);
	    result = RC_OK;
	} else
	    sprintf (RESULT, "Drive %d is disabled.", drive);
    }
    return result;
}

/****************************************************************************/

static void QUIT (void)
{
    uae_quit ();
}

/****************************************************************************/

static int QUERY (const char *line)
{
    const char *res = NULL;

    if      (matchstr (&line, "LED_POW"))      res = led_state[0] ? "1" : "0";
    else if (matchstr (&line, "LED_DF0"))      res = led_state[1] ? "1" : "0";
    else if (matchstr (&line, "LED_DF1"))      res = led_state[2] ? "1" : "0";
    else if (matchstr (&line, "LED_DF2"))      res = led_state[3] ? "1" : "0";
    else if (matchstr (&line, "LED_DF3"))      res = led_state[4] ? "1" : "0";
    else if (matchstr (&line, "NAME_DF0"))     res = currprefs.df[0];
    else if (matchstr (&line, "NAME_DF1"))     res = currprefs.df[1];
    else if (matchstr (&line, "NAME_DF2"))     res = currprefs.df[2];
    else if (matchstr (&line, "NAME_DF3"))     res = currprefs.df[3];
    else if (matchstr (&line, "FAKEJOYSTICK")) res = 0; //res = currprefs.fake_joystick ? "1" : "0";
    else if (matchstr (&line, "DISPLAY"))      res = inhibit_frame ? "0" : "1";
    else if (matchstr (&line, "FRAMERATE")) {
	sprintf (RESULT, "%d", currprefs.gfx_framerate);
        return RC_OK;
    } else if (matchstr (&line, "FRAMENUM")) {
#ifdef USE_AMIGA_GFX
	sprintf (RESULT, "%u", frame_num);
#else
	sprintf (RESULT, "%d", 0);
#endif
	return RC_OK;
    } else if (matchstr (&line, "SOUND")) {
	sprintf (RESULT, "%d", sound_available ? currprefs.produce_sound : -1);
	return RC_OK;
    } else return RC_ERROR;

    if (res)
	strncpy (RESULT, res, RESULT_LEN);

    return RC_OK;
}

/****************************************************************************/

static int FEEDBACK (const char *line)
{
    gui_rexx_s *gui = NULL;

    if      (matchstr (&line, "LED_POW"))  gui = &gui_rexx[LED_POW];
    else if (matchstr (&line, "LED_DF0"))  gui = &gui_rexx[LED_DF0];
    else if (matchstr (&line, "LED_DF1"))  gui = &gui_rexx[LED_DF1];
    else if (matchstr (&line, "LED_DF2"))  gui = &gui_rexx[LED_DF2];
    else if (matchstr (&line, "LED_DF3"))  gui = &gui_rexx[LED_DF3];
    else if (matchstr (&line, "NAME_DF0")) gui = &gui_rexx[NAME_DF0];
    else if (matchstr (&line, "NAME_DF1")) gui = &gui_rexx[NAME_DF1];
    else if (matchstr (&line, "NAME_DF2")) gui = &gui_rexx[NAME_DF2];
    else if (matchstr (&line, "NAME_DF3")) gui = &gui_rexx[NAME_DF3];
    else if (matchstr (&line, "ON_EXIT"))  gui = &gui_rexx[ON_EXIT];
    else return RC_ERROR;

    while(1) {
	if (matchstr (&line, "ADDRESS") ||
	    matchstr (&line, "PORT")) {
	    extractstr (&line, gui->port, PORT_LEN);
	} else if (matchstr (&line,"COMMAND") ||
		   matchstr (&line,"CMD") ||
		   matchstr (&line,"CMD_ON")) {
	    extractstr (&line, gui->cmd_on,  CMD_LEN);
	} else if (matchstr(&line,"CMD_OFF")) {
	    extractstr(&line, gui->cmd_off, CMD_LEN);
	} else break;
    }
    return RC_OK;
}

/****************************************************************************/

static int GET_VERSION (const char *line)
{
    if (matchstr (&line, "STRING")) {
	sprintf (RESULT,
		 PACKAGE_NAME "-" PACKAGE_VERSION " by Richard Drummond & contributors, "
		 "Amiga Port based on work by Samuel Devulder.");
    } else if (matchstr (&line, "NUM")) {
	sprintf (RESULT, "%d", UAEMAJOR * 10000 + UAEMINOR * 100 + UAESUBREV);
    } else if (matchstr (&line, "AUTHOR")) {
	sprintf (RESULT," by Richard Drummond & contributors");
    } else if (matchstr (&line, "PORT")) {
	sprintf (RESULT, "Amiga Port based on work by Samuel Devulder");
    } else return RC_ERROR;
    return RC_OK;
}

/****************************************************************************/

static int FRAMERATE (const char *line)
{
    int num;
    num = matchnum (&line);
    if (num >= 1 && num <= 20)
	changed_prefs.gfx_framerate = num;
    else {
	sprintf (RESULT,"Invalid frame rate: %d\n", num);
	return RC_WARN;
    }
    return RC_OK;
}

/****************************************************************************/

static int FAKEJOYSTICK (const char *line)
{
/*    if     (matchstr(&line,"ON"))     changed_prefs.fake_joystick = 2;
    else if(matchstr(&line,"OFF"))    changed_prefs.fake_joystick = 0;
    else if(matchstr(&line,"TOGGLE")) changed_prefs.fake_joystick =
                                      currprefs.fake_joystick?0:2;
    else return RC_ERROR;
    return RC_OK; */

    return 0;
}

/****************************************************************************/

static int DISPLAY (const char *line)
{
    if      (matchstr (&line, "ON"))     clear_inhibit_frame (IHF_SCROLLLOCK);
    else if (matchstr (&line, "OFF"))    set_inhibit_frame (IHF_SCROLLLOCK);
    else if (matchstr (&line, "TOGGLE")) toggle_inhibit_frame (IHF_SCROLLLOCK);
    else return RC_ERROR;
    return RC_OK;
}

/****************************************************************************/

static int SOUND (const char *line)
{
    if (!sound_available) {
	sprintf (RESULT, "Sound not available!");
	return RC_WARN;
    }

    if      (matchstr (&line, "ON"))     currprefs.produce_sound = 2;
    else if (matchstr (&line, "OFF"))    currprefs.produce_sound = 1;
    else if (matchstr (&line, "BEST"))   currprefs.produce_sound = 3;
    else if (matchstr (&line, "TOGGLE"))
	currprefs.produce_sound = currprefs.produce_sound <= 1 ? 2 : 1;
    else return RC_ERROR;

    /* sam: the next 2 lines is a hack and I don't like it */
/*    eventtab[ev_sample].active = (changed_prefs.produce_sound>=2)?1:0;
    events_schedule ();
*/
    return RC_OK;
}

/****************************************************************************/

static int DEBUG (void)
{
#ifdef DEBUGGER
    activate_debugger ();
    return RC_OK;
#else
    return RC_ERROR;
#endif
}

/****************************************************************************/

static int UAEEXE (const char *line)
{
    if (uaeexe (line)) {
	sprintf (RESULT, "Remote CLI failed!");
	return RC_WARN;
    }
    return RC_OK;
}

/****************************************************************************/

static int process_cmd (const char *line)
{
    RESULT[0] = '\0';
    if      (matchstr (&line, "EJECT"))        return EJECT (line);
    else if (matchstr (&line, "INSERT"))       return INSERT (line);
    else if (matchstr (&line, "QUERY"))        return QUERY (line);
    else if (matchstr (&line, "FEEDBACK"))     return FEEDBACK (line);
    else if (matchstr (&line, "VERSION"))      return GET_VERSION (line);
    else if (matchstr (&line, "BYE"))          QUIT ();
    else if (matchstr (&line, "QUIT"))         QUIT ();
    else if (matchstr (&line, "DEBUG"))        return DEBUG ();
    else if (matchstr (&line, "RESET"))        m68k_reset ();
    else if (matchstr (&line, "DISPLAY"))      return DISPLAY (line);
    else if (matchstr (&line, "FRAMERATE"))    return FRAMERATE( line);
    else if (matchstr (&line, "FAKEJOYSTICK")) return FAKEJOYSTICK (line);
    else if (matchstr (&line, "SOUND"))        return SOUND (line);
    else if (matchstr (&line, "UAEEXE"))       return UAEEXE (line);
    else return RC_ERROR;
    return RC_OK;
}

/****************************************************************************/

void rexx_handle_events (void)
{
    struct RexxMsg *msg;
    while ((msg = (struct RexxMsg*)GetMsg (ARexxPort))) {
	if (!(msg->rm_Action & RXCOMM)) {
	    write_log ("Unknown action '%08X' recieved!\n",
		       msg->rm_Action);
	    continue;
	}
	msg->rm_Result1 = process_cmd (msg->rm_Args[0]);
	msg->rm_Result2 = 0;
	if (msg->rm_Action & RXFF_RESULT) {
	    int len = strlen (RESULT); /* holger: trick for powerup */
	    msg->rm_Result2 = (LONG)CreateArgstring (RESULT,len);
	}
	ReplyMsg ((void*)msg);
    }
}

/****************************************************************************/

void rexx_led (int led, int on)
{
    gui_rexx_s *gui = NULL;

    if (led < 0 || led > 4)
	return;

    led_state[led] = on;

    if (led == 0) gui = &gui_rexx[LED_POW];
    if (led == 1) gui = &gui_rexx[LED_DF0];
    if (led == 2) gui = &gui_rexx[LED_DF1];
    if (led == 3) gui = &gui_rexx[LED_DF2];
    if (led == 4) gui = &gui_rexx[LED_DF3];

    if (gui->port[0] && gui->cmd_on[0] && gui->cmd_off[0]) {
	if (ADDRESS (gui->port, on ? gui->cmd_on : gui->cmd_off) != RC_OK) {
	    write_log ("%s:%s:%s\n", gui->port,
				     on ? gui->cmd_on : gui->cmd_off,
				     RESULT);
	}
    }
}

/****************************************************************************/

void rexx_filename (unsigned int num, const char *filename)
{
    gui_rexx_s *gui = NULL;

    if (num > 3)
	return;
    gui = &gui_rexx[NAME_DF0 + num];
    if (gui->port[0] && gui->cmd_on[0]) {
	char buf[CMD_LEN];
	sprintf (buf, gui->cmd_on, filename);
	if (ADDRESS (gui->port, buf) != RC_OK) {
	    write_log ("%s:%s:%s\n", gui->port, buf, RESULT);
	}
    }
}

/****************************************************************************/
/* send a message to an AREXX port.
 */
static int ADDRESS (const char *hostname, const char *cmd)
{
    struct MsgPort *RexxPort,
		   *ReplyPort;
    struct RexxMsg *HostMsg,
		   *answer;
    int result = RC_WARN;

    if (!stricmp (hostname, "COMMAND"))
	return SystemTagList(cmd,NULL);

    if ((RexxPort = (void *)FindPort (hostname))) {
	if ((ReplyPort = (void *)CreateMsgPort ())) {
	    if ((HostMsg = CreateRexxMsg (ReplyPort, NULL, hostname))) {
		unsigned int len = strlen (cmd); /* holger: trick for powerup */
		if ((HostMsg->rm_Args[0] = CreateArgstring ((char *)cmd, len))) {
		    HostMsg->rm_Action = RXCOMM | RXFF_RESULT;
		    PutMsg (RexxPort, (void*)HostMsg);
		    WaitPort (ReplyPort);
		    while (!(answer = (void *)GetMsg (ReplyPort)));
		    result = answer->rm_Result1;
		    if (result == RC_OK) {
			if (answer->rm_Result2) {
			    strncpy (RESULT,(char *)answer->rm_Result2, RESULT_LEN);
			    DeleteArgstring ((char *)answer->rm_Result2);
			} else RESULT[0] = '\0';
		    }
		    DeleteArgstring (HostMsg->rm_Args[0]);
		} else strcpy (RESULT, "Can't create argstring!");
		DeleteRexxMsg (HostMsg);
	    } else strcpy (RESULT, "Can't create rexx message!");
	    DeleteMsgPort (ReplyPort);
	} else strcpy (RESULT, "Can't alloc reply port!");
    } else sprintf (RESULT, "Port \"%s\" not found!", hostname);
    return result;
}

/****************************************************************************/
/* argument parsing routines
 */
static int matchstr (const char **line, const char *pat)
{
    const char *s = *line;
    char match = 0;

    while (isspace (*s))
	++s;
    if (*s == '\"' || *s == '\'')
	match = *s++;
    while (*s && (tolower (*s) == tolower (*pat)) && (!match || *s != match))
	{++s;++pat;}
    if (match && *s == match && s[1])
	++s;
    if (!*pat && (!*s || isspace (*s))) {
	while (isspace (*s))
	    ++s;
	*line = s;
	return 1;
    }
    return 0;
}

/****************************************************************************/

static void extractstr (const char **line, char *result, unsigned int len)
{
    const char *s = *line;
    char match = 0;

    while (isspace(*s))
	++s;

    if (*s == '\"' || *s == '\'')
	match = *s++;
    while (*s && *s != match) {
	if (*s == '\\' && (s[1] == '\'' || s[1] == '\"'))
	    ++s;
	if (len > 1) {
	    *result++ = *s;
	    --len;
	}
	++s;
	if (!match && isspace (*s))
	    break;
    }
    if (match && *s == match)
	++s;
    while (isspace (*s))
	++s;

    *result  = '\0';
    *line    = s;
}

/****************************************************************************/

static int matchnum (const char **line)
{
    const char *s = *line;
    char match = 0;
    int sign = 1, num = 0;

    while (isspace (*s))
	++s;
    if (*s == '\"' || *s == '\'')
	match = *s++;
    if (*s == '-') {
	sign = -1;
	++s;
    }
    if (*s == '+')
	++s;
    while (isspace (*s))
	++s;
    while (*s >= '0' && *s <= '9')
	num = num * 10 + (*s++ - '0');
    if (match && *s == match && s[1])
	++s;
    while (isspace (*s))
	++s;
    *line = s;
    return sign > 0 ? num : -num;
}

/****************************************************************************/

#ifdef POWERUP
/* sam: those function should be in the ppc version of the unexisting
        libamiga.a */
#define NEWLIST(l) ((l)->lh_Head = (struct Node *)&(l)->lh_Tail, \
		    /*(l)->lh_Tail = NULL,*/ \
		    (l)->lh_TailPred = (struct Node *)&(l)->lh_Head)

struct MsgPort *CreatePort (STRPTR name, LONG pri)
{
    struct MsgPort *port = NULL;
    UBYTE portsig;

    if ((BYTE)(portsig = AllocSignal (-1)) >= 0) {
	if (!(port = PPCAllocMem (sizeof (struct MsgPort), MEMF_CLEAR | MEMF_PUBLIC)))
	    FreeSignal(portsig);
	else {
	    port->mp_Node.ln_Type = NT_MSGPORT;
	    port->mp_Node.ln_Pri  = pri;
	    port->mp_Node.ln_Name = name;
	    /* done via AllocMem
	     * port->mp_Flags = PA_SIGNAL;
	     */
	    port->mp_SigBit  = portsig;
	    port->mp_SigTask = FindTask (NULL);
	    NEWLIST (&port->mp_MsgList);
	    if (port->mp_Node.ln_Name)
		AddPort(port);
	}
    }
    return port;
}

void DeletePort (struct MsgPort *port)
{
    int i;

    if (port->mp_Node.ln_Name != NULL)
	RemPort (port);
    i = -1;
    port->mp_Node.ln_Type = i;
    port->mp_MsgList.lh_Head = (struct Node *)i;
    FreeSignal (port->mp_SigBit);
    PPCFreeMem (port, sizeof (struct MsgPort));
}

struct IORequest *CreateExtIO (struct MsgPort *port, long iosize)
{
    struct IORequest *ioreq=NULL;

    if (port && (ioreq = PPCAllocMem (iosize, MEMF_CLEAR | MEMF_PUBLIC))) {
	ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	ioreq->io_Message.mn_ReplyPort    = port;
	ioreq->io_Message.mn_Length       = iosize;
    }
    return ioreq;
}

struct IOStdReq *CreateStdIO (struct MsgPort *port)
{
    return (struct IOStdReq *)CreateExtIO (port, sizeof (struct IOStdReq));
}

void DeleteExtIO (struct IORequest *ioreq)
{
    int i;

    i=-1;
    ioreq->io_Message.mn_Node.ln_Type = i;
    ioreq->io_Device = (struct Device *)i;
    ioreq->io_Unit = (struct Unit *)i;
    PPCFreeMem (ioreq, ioreq->io_Message.mn_Length);
}

void DeleteStdIO (struct IORequest *ioreq)
{
    DeleteExtIO (ioreq);
}

#endif
