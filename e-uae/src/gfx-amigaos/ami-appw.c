 /*
  * UAE - The Un*x Amiga Emulator
  *
  * AppWindow interface
  *
  * Copyright 1997 Samuel Devulder.
  */

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "uaeexe.h"

/****************************************************************************/

#include <workbench/startup.h>

#include <exec/execbase.h>
#include <exec/memory.h>

#include <dos/dos.h>

#if defined(POWERUP)
# ifndef USE_CLIB
#  include <powerup/ppcproto/intuition.h>
#  include <powerup/ppcproto/exec.h>
#  include <powerup/ppcproto/dos.h>
/*#include <workbench/workbench.h> sam: a effacer*/
#  include <powerup/ppcproto/wb.h>
# else
#  include <clib/intuition_protos.h>
#  include <clib/exec_protos.h>
#  include <clib/dos_protos.h>
#  include <clib/wb_protos.h>
# endif

# define AddAppWindow(a0, a1, a2, a3, tags...) (\
{ \
    ULONG _tags[] = {tags}; \
    AddAppWindowA ((a0), (a1), (a2), (a3), (struct TagItem *)_tags); \
})

#else
# include <proto/intuition.h>
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/wb.h>
# ifndef __amigaos4__
#  include <clib/alib_protos.h>
# endif
#endif

/****************************************************************************/

#define LEN 256

struct Library		*WorkbenchBase;
#ifdef __amigaos4__
struct WorkbenchIFace	*IWorkbench;
#endif

static struct AppWindow *AppWin;
static struct MsgPort   *AppPort;

int  appw_init   (struct Window *W);
void appw_exit   (void);
void appw_events (void);

/****************************************************************************/

int appw_init (struct Window *W)
{
    WorkbenchBase = (void*) OpenLibrary ("workbench.library", 36L);
    if (WorkbenchBase) {
#ifdef __amigaos4__
	IWorkbench = (struct WorkbenchIFace *)
		     GetInterface (WorkbenchBase, "main", 1, NULL);
	if (IWorkbench) {
#endif
	    AppPort = CreatePort (0, 0);
	    if (AppPort) {
		AppWin = AddAppWindow (0, 0, W, AppPort, NULL);
		if (AppWin) {
		     write_log ("AppWindow started.\n");
		     return 1;
		}
	    }
#ifdef __amigaos4__
	}
	CloseLibrary (WorkbenchBase);
	WorkbenchBase = 0;
#endif
    }
    write_log ("Failed to start AppWindow.\n");

    return 0;
}

/****************************************************************************/

void appw_exit (void)
{
    if (AppPort) {
	void *msg;
	while ((msg = GetMsg (AppPort)))
	    ReplyMsg (msg);
	DeletePort (AppPort);
	AppPort = NULL;
    }
    if (AppWin) {
	RemoveAppWindow (AppWin);
	AppWin = NULL;
    }
    if (WorkbenchBase) {
	CloseLibrary (WorkbenchBase);
	WorkbenchBase = NULL;
    }
}

/***************************************************************************/

static void addcmd (char *cmd, int len, ULONG lock, char *name)
{
    len -= strlen (cmd);
    if (len <= 0)
	return;

    while (*cmd)
	++cmd;

    *cmd++ = ' ';
    if (--len <= 0)
	return;

    NameFromLock (lock, cmd, len);
    len -= strlen (cmd);
    if (len <= 0)
	return;
    while (*cmd)
	++cmd;
    if (cmd[-1] != ':') {
	*cmd++='/';
	len -= 1;
	if (len<=0)
	    return;
    }
    strncpy (cmd, name, len);
    while (*cmd) {
	++cmd;
	if (--len <= 0)
	*cmd='\0';
    }
}

/***************************************************************************/

void appw_events (void)
{
    if (AppWin) {
	struct AppMessage *msg;
	struct WBArg *arg;
	while ((msg = (void*) GetMsg (AppPort))) {
	    char cmd[LEN];
	    int  i;

	    arg = msg->am_ArgList;

	    strcpy (cmd, "cd ");
	    NameFromLock (arg[0].wa_Lock, &cmd[3], LEN - 3);
	    if (!uaeexe (cmd)) {
		strcpy (cmd,"run "); strcpy (cmd + 4, arg[0].wa_Name);
		for (i = 1; i < msg->am_NumArgs; ++i)
		    addcmd (cmd, LEN - 2, arg[i].wa_Lock, arg[i].wa_Name);
		    /*                 ^    */
		    /* 2 bytes for security */
		uaeexe (cmd);
	    }
	    ReplyMsg ((void*) msg);
	}
    }
}
