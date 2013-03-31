/*
 * UAE - The Un*x Amiga Emulator
 *
 * Copyright 2004 Richard Drummond
 *
 * Start-up and support functions for BeOS target
 */

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "xwin.h"
#include "debug.h"

#include "signal.h"

#ifdef USE_SDL
# include <SDL.h>
#endif
}

#ifndef USE_SDL
# include "gfxdep/be-UAE.h"
#endif


#ifndef USE_SDL
int  argcCopy;
char **argvCopy;

void freeCopiedArgs();
void copyArgs(int argc,char **argv);

void freeCopiedArgs()
{
	for(int i = 0;i < argcCopy;i++)
		free(argvCopy[i]);

	delete[] argvCopy;
}

void copyArgs (int argc,char **argv)
{
	argcCopy = argc;
	argvCopy = new char *[argcCopy+1];

	for(int i = 0;i < argc;i++)
		argvCopy[i] = strdup(argv[i]);

	argvCopy[argc] = NULL;
}
#endif

/*
 * BeOS-specific main entry
 */
int main (int argc, char *argv[])
{
#ifndef USE_SDL
/*
 * If we're not using SDL for anything, we would initialize our
 * application object here. On the other hand, if you are using SDL,
 * we can't because SDL creates its own application object.
 */
	// Copy command line arguments
	copyArgs(argc,argv);

    UAE uae;
    uae.Run();

	freeCopiedArgs();

	// restore original workspace resolution (if it has changed)
	restoreWorkspaceResolution();
#else
    real_main(argc, argv);
#endif
    return 0;
}

/*
 * Handle CTRL-C signals
 */
static void sigbrkhandler (int foo)
{
#ifdef DEBUGGER
    activate_debugger ();
#endif
}

void setup_brkhandler (void)
{
    struct sigaction sa;
    sa.sa_handler = sigbrkhandler;
    sa.sa_flags = 0;
    sigemptyset (&sa.sa_mask);
    sigaction (SIGINT, &sa, NULL);
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
