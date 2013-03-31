/*
 * E-UAE - The portable Amiga emulator
 *
 * Copyright 2004-2006 Richard Drummond
 *
 * Start-up and support functions for Amiga target
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "xwin.h"
#include "debug.h"

#include "signal.h"

#define  __USE_BASETYPE__
#include <proto/exec.h>
#undef   __USE_BASETYPE__
#include <exec/execbase.h>

#ifdef USE_SDL
# include <SDL.h>
#endif

/* Get compiler/libc to enlarge stack to this size - if possible */
#if defined __PPC__ || defined __ppc__ || defined POWERPC || defined __POWERPC__
# define MIN_STACK_SIZE  (64 * 1024)
#else
# define MIN_STACK_SIZE  (32 * 1024)
#endif

#if defined __libnix__ || defined __ixemul__
/* libnix requires that we link against the swapstack.o module */
unsigned int __stack = MIN_STACK_SIZE;
#else
# if !defined __MORPHOS__ && !defined __AROS__
/* clib2 minimum stack size. Use this on OS3.x and OS4.0. */
unsigned int __stack_size = MIN_STACK_SIZE;
# endif
#endif

struct Device *TimerBase;
#ifdef __amigaos4__
struct Library *ExpansionBase;
struct TimerIFace *ITimer;
struct ExpansionIFace *IExpansion;
#endif

static void free_libs (void)
{
#ifdef __amigaos4__
    if (ITimer)
	DropInterface ((struct Interface *)ITimer);
    if (IExpansion)
	DropInterface ((struct Interface *)IExpansion);
    if (ExpansionBase)
	CloseLibrary (ExpansionBase);
#endif
}

static void init_libs (void)
{
    atexit (free_libs);

    TimerBase = (struct Device *) FindName(&SysBase->DeviceList, "timer.device");

#ifdef __amigaos4__
    ITimer = (struct TimerIFace *) GetInterface((struct Library *)TimerBase, "main", 1, 0);

    ExpansionBase = OpenLibrary ("expansion.library", 0);
    if (ExpansionBase)
	IExpansion = (struct ExpansionIFace *) GetInterface(ExpansionBase, "main", 1, 0);

    if(!ITimer || !IExpansion)
	exit (20);
#endif
}

static int fromWB;
static FILE *logfile;

/*
 * Amiga-specific main entry
 */
int main (int argc, char *argv[])
{
    fromWB = argc == 0;

    if (fromWB)
	set_logfile ("T:E-UAE.log");

    init_libs ();

#ifdef USE_SDL
    init_sdl ();
#endif

    real_main (argc, argv);

    if (fromWB)
	set_logfile (0);

    return 0;
}

/*
 * Handle CTRL-C signals
 */
static RETSIGTYPE sigbrkhandler(int foo)
{
#ifdef DEBUGGER
    activate_debugger ();
#endif
}

void setup_brkhandler (void)
{
#ifdef HAVE_SIGACTION
    struct sigaction sa;
    sa.sa_handler = (void*)sigbrkhandler;
    sa.sa_flags = 0;
    sa.sa_flags = SA_RESTART;
    sigemptyset (&sa.sa_mask);
    sigaction (SIGINT, &sa, NULL);
#else
    signal (SIGINT,sigbrkhandler);
#endif
}


/*
 * Handle target-specific cfgfile options
 */
void target_save_options (FILE *f, const struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p)
{
}
