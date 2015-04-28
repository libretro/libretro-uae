/*
 * UAE - The Un*x Amiga Emulator
 *
 * Copyright 2004 Richard Drummond
 *
 * Start-up and support functions used by Linux/Unix targets
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "debug.h"

/* external prototypes */
extern int scan_roms (int);
#undef __unix

/*
 * Handle break signal
 */
#ifndef __CELLOS_LV2__
#include <signal.h>
#endif

#ifdef __cplusplus
static RETSIGTYPE sigbrkhandler(...)
#else
static RETSIGTYPE sigbrkhandler (int foo)
#endif
{
#ifdef DEBUGGER

    activate_debugger ();
#endif

#ifndef __CELLOS_LV2__
#if !defined(__unix) || defined(__NeXT__) 
    signal (SIGINT, sigbrkhandler);
#endif
#endif
}

int debuggable (void)
{
    return 1;
}


void setup_brkhandler (void)
{
#ifndef __CELLOS_LV2__
#if defined(__unix) && !defined(__NeXT__)
    struct sigaction sa;
    sa.sa_handler = sigbrkhandler;
    sa.sa_flags = 0;
#ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
#endif
    sigemptyset (&sa.sa_mask);
    sigaction (SIGINT, &sa, NULL);
#else
    signal (SIGINT, sigbrkhandler);
#endif
#endif
}
#if 0
/*
 * Handle target-specific cfgfile options
 */
void target_save_options (struct zfile *f, struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const TCHAR *option, const TCHAR *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
}
#endif
