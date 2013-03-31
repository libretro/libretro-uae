 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Solaris sound
  *
  * Copyright 1996, 1997 Manfred Thole
  * Copyright 2007       Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "memory.h"
#include "events.h"
#include "custom.h"
#include "audio.h"
#include "gensound.h"
#include "sounddep/sound.h"

#if !defined HAVE_SYS_AUDIOIO_H

/* SunOS 4.1.x */
#include <sys/ioctl.h>
#include <sun/audioio.h>
#ifndef AUDIO_ENCODING_LINEAR
#define AUDIO_ENCODING_LINEAR (3)
#endif

#else

/* SunOS 5.x/NetBSD */
#include <sys/audioio.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif

int sndbufsize;
int sound_fd;
static int have_sound;
uae_u16 sndbuffer[44100];
uae_u16 *sndbufpt;

static void clearbuffer (void)
{
    memset (sndbuffer, 0 , sizeof (sndbuffer));
}

void close_sound(void)
{
    if (have_sound)
	close (sound_fd);
}

int setup_sound (void)
{
    sound_fd = open ("/dev/audio", O_WRONLY);
    have_sound = !(sound_fd < 0);
    if (!have_sound)
      return 0;

    sound_available = 1;
    return 1;
}

int init_sound (void)
{
    int rate, dspbits;

    struct audio_info sfd_info;

    rate = currprefs.sound_freq;
    dspbits = currprefs.sound_bits;
    AUDIO_INITINFO(&sfd_info);
    sfd_info.play.sample_rate = rate;
    sfd_info.play.channels = 1;
    sfd_info.play.precision = dspbits;
    sfd_info.play.encoding = (dspbits == 8 ) ? AUDIO_ENCODING_ULAW : AUDIO_ENCODING_LINEAR;
    if (ioctl (sound_fd, AUDIO_SETINFO, &sfd_info)) {
	write_log ("Can't use sample rate %d with %d bits, %s.\n", rate, dspbits, (dspbits == 8) ? "ulaw" : "linear");
	return 0;
    }

    init_sound_table16 ();

    if (dspbits == 8)
	sample_handler = sample_ulaw_handler;
    else
	sample_handler = sample16_handler;

    sndbufpt = sndbuffer;
    obtainedfreq = rate;

    sound_available = 1;
    sndbufsize = rate * currprefs.sound_latency * (dspbits / 8) * (currprefs.sound_stereo ? 2 : 1) / 1000;
    sndbufsize = (sndbufsize + 1) & ~1;

    write_log ("Sound driver found and configured for %d bits, %s at %d Hz, buffer is %d bytes.\n",
	       dspbits, (dspbits ==8) ? "ulaw" : "linear", rate, sndbufsize);
    return 1;
}

void reset_sound (void)
{
   clearbuffer ();
}

void pause_sound (void)
{
   clearbuffer ();
}

void resume_sound (void)
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
