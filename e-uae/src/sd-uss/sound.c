 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Linux/USS sound
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2003-2007 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gensound.h"
#include "sounddep/sound.h"

#include <sys/ioctl.h>

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#elif defined HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#else
#error "Something went wrong during configuration."
#endif

int sound_fd;
static int have_sound = 0;
static unsigned long formats;

uae_u16 sndbuffer[44100];
uae_u16 *sndbufpt;
int sndbufsize;

static int exact_log2 (int v)
{
    int l = 0;
    while ((v >>= 1) != 0)
	l++;
    return l;
}

void close_sound (void)
{
    if (have_sound)
	close (sound_fd);
}

/* Try to determine whether sound is available.  This is only for GUI purposes.  */
int setup_sound (void)
{
    sound_fd = open ("/dev/dsp", O_WRONLY);

    sound_available = 0;

    if (sound_fd < 0) {
	perror ("Can't open /dev/dsp");
	if (errno == EBUSY) {
	    /* We can hope, can't we ;) */
	    sound_available = 1;
	    return 1;
	}
	return 0;
    }

    if (ioctl (sound_fd, SNDCTL_DSP_GETFMTS, &formats) == -1) {
	perror ("ioctl failed - can't use sound");
	close (sound_fd);
	return 0;
    }

    sound_available = 1;
    close (sound_fd);
    return 1;
}

int init_sound (void)
{
    int tmp;
    int rate;
    int dspbits;
    int bufsize;

    sound_fd = open ("/dev/dsp", O_WRONLY);
    have_sound = !(sound_fd < 0);
    if (! have_sound) {
	perror ("Can't open /dev/dsp");
	if (errno != EBUSY)
	    sound_available = 0;
	return 0;
    }
    if (ioctl (sound_fd, SNDCTL_DSP_GETFMTS, &formats) == -1) {
	perror ("ioctl failed - can't use sound");
	close (sound_fd);
	have_sound = 0;
	return 0;
    }

    dspbits = currprefs.sound_bits;
    ioctl (sound_fd, SNDCTL_DSP_SAMPLESIZE, &dspbits);
    ioctl (sound_fd, SOUND_PCM_READ_BITS, &dspbits);
    if (dspbits != currprefs.sound_bits) {
	fprintf (stderr, "Can't use sound with %d bits\n", currprefs.sound_bits);
	return 0;
    }

    tmp = currprefs.sound_stereo;
    ioctl (sound_fd, SNDCTL_DSP_STEREO, &tmp);

    rate = currprefs.sound_freq;
    ioctl (sound_fd, SNDCTL_DSP_SPEED, &rate);
    ioctl (sound_fd, SOUND_PCM_READ_RATE, &rate);
    /* Some soundcards have a bit of tolerance here. */
    if (rate < currprefs.sound_freq * 90 / 100 || rate > currprefs.sound_freq * 110 / 100) {
	fprintf (stderr, "Can't use sound with desired frequency %d\n", currprefs.sound_freq);
	return 0;
    }

    bufsize = rate * currprefs.sound_latency * (dspbits / 8) * (currprefs.sound_stereo ? 2 : 1) / 1000;

    tmp = 0x00040000 + exact_log2 (bufsize);
    ioctl (sound_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
    ioctl (sound_fd, SNDCTL_DSP_GETBLKSIZE, &sndbufsize);

    obtainedfreq = currprefs.sound_freq;

    if (dspbits == 16) {
	/* Will this break horribly on bigendian machines? Possible... Not any more - Rich */
	if (!(formats & AFMT_S16_NE))
	    return 0;
	init_sound_table16 ();
	sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;
    } else {
	if (!(formats & AFMT_U8))
	    return 0;
	init_sound_table8 ();
	sample_handler = currprefs.sound_stereo ? sample8s_handler : sample8_handler;
    }
    sound_available = 1;
    printf ("Sound driver found and configured for %d bits at %d Hz, buffer is %d bytes (%d ms).\n",
	    dspbits, rate, sndbufsize, sndbufsize * 1000 / (rate * dspbits / 8 * (currprefs.sound_stereo ? 2 : 1)));
    sndbufpt = sndbuffer;

    return 1;
}

void reset_sound (void)
{
}

void pause_sound (void)
{
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
