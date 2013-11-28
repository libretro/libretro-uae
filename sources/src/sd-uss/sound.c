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
#include "gui.h"
#include "audio.h"
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
unsigned int have_sound = 0;
static unsigned long formats;
static int statuscnt;

uae_u16 paula_sndbuffer[44100];
uae_u16 *paula_sndbufpt;
int paula_sndbufsize;
static struct sound_data sdpaula;
static struct sound_data *sdp = &sdpaula;

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

void finish_sound_buffer (void)
{
	if (currprefs.turbo_emulation)
		return;
#ifdef DRIVESOUND
	driveclick_mix ((uae_s16*)paula_sndbuffer, paula_sndbufsize / 2, currprefs.dfxclickchannelmask);
#endif
	if (!have_sound)
		return;
	if (statuscnt > 0) {
		statuscnt--;
		if (statuscnt == 0)
			gui_data.sndbuf_status = 0;
	}
	if (gui_data.sndbuf_status == 3)
		gui_data.sndbuf_status = 0;
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

#ifdef DRIVESOUND
	driveclick_init();
#endif
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

    dspbits = 16;
    ioctl (sound_fd, SNDCTL_DSP_SAMPLESIZE, &dspbits);
    ioctl (sound_fd, SOUND_PCM_READ_BITS, &dspbits);
    if (dspbits != 16) {
		fprintf (stderr, "Can't use sound with %d bits\n", dspbits);
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

    bufsize = rate * (dspbits / 8) * (currprefs.sound_stereo ? 2 : 1) / 1000;

    tmp = 0x00040000 + exact_log2 (bufsize);
    ioctl (sound_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
    ioctl (sound_fd, SNDCTL_DSP_GETBLKSIZE, &paula_sndbufsize);

    obtainedfreq = currprefs.sound_freq;

    if (!(formats & AFMT_S16_NE))
		return 0;
    sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;

    sound_available = 1;
    printf ("Sound driver found and configured for %d bits at %d Hz, buffer is %d bytes (%d ms).\n",
	    dspbits, rate, paula_sndbufsize, paula_sndbufsize * 1000 / (rate * dspbits / 8 * (currprefs.sound_stereo ? 2 : 1)));
    paula_sndbufpt = paula_sndbuffer;

#ifdef DRIVESOUND
	driveclick_reset ();
#endif

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

void set_volume_sound_device (struct sound_data *sd, int volume, int mute)
{
}

void set_volume (int volume, int mute)
{
        set_volume_sound_device (sdp, volume, mute);
        config_changed = 1;
}

static int setget_master_volume_linux (int setvolume, int *volume, int *mute)
{
        unsigned int ok = 0;

        if (setvolume) {
                ;//set
        } else {
                ;//get
        }

        return ok;
}

static int set_master_volume (int volume, int mute)
{
        return setget_master_volume_linux (1, &volume, &mute);
}

static int get_master_volume (int *volume, int *mute)
{
        *volume = 0;
        *mute = 0;
        return setget_master_volume_linux (0, volume, mute);
}

void master_sound_volume (int dir)
{
        int vol, mute, r;

        r = get_master_volume (&vol, &mute);
        if (!r)
                return;
        if (dir == 0)
                mute = mute ? 0 : 1;
        vol += dir * (65536 / 10);
        if (vol < 0)
                vol = 0;
        if (vol > 65535)
                vol = 65535;
        set_master_volume (vol, mute);
        config_changed = 1;
}

void sound_mute (int newmute)
{
        if (newmute < 0)
                sdp->mute = sdp->mute ? 0 : 1;
        else
                sdp->mute = newmute;
        set_volume (currprefs.sound_volume, sdp->mute);
        config_changed = 1;
}

void sound_volume (int dir)
{
/*
    currprefs.sound_volume -= dir * 10;
    if (currprefs.sound_volume < 0)
		currprefs.sound_volume = 0;
    if (currprefs.sound_volume > 100)
		currprefs.sound_volume = 100;
    changed_prefs.sound_volume = currprefs.sound_volume;
    set_volume (currprefs.sound_volume, sdp->mute);
*/
}

void restart_sound_buffer (void)
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
