 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Amiga audio.device sound
  *
  * Copyright 1996, 1997, 1998 Samuel Devulder, Holger Jakob (AHI).
  * Copyright 2004-2007 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gensound.h"
#include "sounddep/sound.h"

#include <exec/memory_uae.h>
#include <graphics/gfxbase.h>

struct MsgPort    *AHImp    = NULL;
struct AHIRequest *AHIio[2] = {NULL, NULL};
struct AHIRequest *linkio   = NULL;


unsigned char *buffers[2];
uae_u16 *paula_sndbuffer;
uae_u16 *paula_sndbufpt;
int paula_sndbufsize;
int bufidx;

unsigned int have_sound;
int clockval;
int period;



int setup_sound (void)
{
    sound_available = 1;
    return 1;
}

static BOOL open_AHI (void)
{
    if ((AHImp = CreateMsgPort())) {
	if ((AHIio[0] = (struct AHIRequest *)
		CreateIORequest (AHImp, sizeof (struct AHIRequest)))) {
	    AHIio[0]->ahir_Version = 4;

	    if (!OpenDevice (AHINAME, 0, (struct IORequest *)AHIio[0], 0)) {
		if ((AHIio[1] = malloc (sizeof(struct AHIRequest)))) {
		    memcpy (AHIio[1], AHIio[0], sizeof(struct AHIRequest));
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
}

static void close_AHI (void)
{
    if (!CheckIO ((struct IORequest *)AHIio[0]))
	WaitIO ((struct IORequest *)AHIio[0]);

    if (linkio) { /* Only if the second request was started */
	if (!CheckIO ((struct IORequest *) AHIio[1]))
	    WaitIO ((struct IORequest *) AHIio[1]);
    }
    CloseDevice ((struct IORequest *) AHIio[0]);
    DeleteIORequest ((void*) AHIio[0]);
    xfree (AHIio[1]);
    DeleteMsgPort ((void*)AHImp);
    AHIio[0] = NULL;
    AHIio[1] = NULL;
    linkio   = NULL;
}

static int get_clockval (void)
{
    struct GfxBase *GB;
    int clk = 0;

    GB = (void*) OpenLibrary ("graphics.library", 0L);

    if (GB) {
	if (GB->DisplayFlags & PAL)
	    clk = 3546895;        /* PAL clock */
	else
	    clk = 3579545;        /* NTSC clock */
	CloseLibrary ((void *) GB);
    }
    return clk;
}

int init_sound (void)
{
    /* too complex ? No it is only the allocation of a single channel ! */
    /* it would have been far less painfull if AmigaOS provided a */
    /* SOUND: device handler */
    int rate;

    atexit (close_sound); /* if only amiga os had resource tracking */

    /* determine the clock */
    clockval = get_clockval ();
    if (clockval == 0)
	goto fail;

    /* check freq */
    if (!currprefs.sound_freq)
	currprefs.sound_freq = 22000;
    if (clockval / currprefs.sound_freq < 80/*124*/ || clockval/currprefs.sound_freq > 65535) {
	write_log ("Can't use sound with desired frequency %d Hz\n", currprefs.sound_freq);
	changed_prefs.sound_freq = currprefs.sound_freq = 22000;
    }
    rate   = currprefs.sound_freq;
    period = (uae_u16)(clockval / rate);

    if (!open_AHI ())
	goto fail;

    /* calculate buffer size */
    paula_sndbufsize = rate * currprefs.sound_latency * 2 * (currprefs.sound_stereo ? 2 : 1) / 1000;
    paula_sndbufsize = (paula_sndbufsize + 1) & ~1;

    /* get the buffers */
    buffers[0] = (void*) AllocMem (paula_sndbufsize,MEMF_PUBLIC | MEMF_CLEAR);
    buffers[1] = (void*) AllocMem (paula_sndbufsize,MEMF_PUBLIC | MEMF_CLEAR);
    if (!buffers[0] || !buffers[1])
	goto fail;

    bufidx    = 0;
    paula_sndbuffer = paula_sndbufpt = (uae_u16*) buffers[bufidx];

    sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;

    have_sound = 1;
    obtainedfreq = rate;

    write_log ("Sound driver found and configured for %s "
	       "at %d Hz, buffer is %d bytes.\n",
	       currprefs.sound_stereo ? "stereo" : "mono",
	       rate, paula_sndbufsize);

    sound_available = 1;
    return 1;
fail:
    sound_available = 0;
    return 0;
}

void close_sound (void)
{
    close_AHI ();

    if (buffers[0]) {
	FreeMem ((APTR) buffers[0], paula_sndbufsize);
	buffers[0] = 0;
    }
    if (buffers[1]) {
	FreeMem ((APTR) buffers[1], paula_sndbufsize);
	buffers[1] = 0;
    }
    if (sound_available) {
	sound_available = 0;
    }
}

void pause_sound (void)
{
}

void resume_sound (void)
{
}

void reset_sound (void)
{
}

void sound_volume (int dir)
{
}

/*
 * Handle audio specific cfgfile options
 */
void audio_default_options (struct uae_prefs *p)
{
}

void audio_save_options (FILE *f, const struct uae_prefs *p)
{
}

int audio_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}
