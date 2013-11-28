 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Linux/ALSA sound
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2004 Heikki Orsila
  * Copyright 2006-2007 Richard Drummond
  *
  * BUGS: certainly
  * TODO:
  * - if setup_sound() fails, there may still be hope to get the
  *   sound device, but we totally give up.. see sd-uss.
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "cfgfile.h"
#include "gensound.h"
#include "sounddep/sound.h"
#include "driveclick.h"

#include <alsa/asoundlib.h>

/* internal types */
char alsa_device[256];
bool alsa_verbose;

unsigned int have_sound = 0;

uae_u16 paula_sndbuffer[44100];
uae_u16 *paula_sndbufpt;
int paula_sndbufsize;

snd_pcm_t *alsa_playback_handle = 0;
int bytes_per_frame;

static struct sound_data sdpaula;
static struct sound_data *sdp = &sdpaula;

/* internal prototypes */
void audio_default_options (struct uae_prefs *);
void audio_save_options (struct zfile *, const struct uae_prefs *);
int audio_parse_option (struct uae_prefs *, const char *, const char *);
void set_volume_sound_device (struct sound_data *, int, int);
void set_volume (int, int);
void master_sound_volume (int);
void sound_mute (int);


void close_sound (void)
{
  if (alsa_playback_handle) {
    snd_pcm_close (alsa_playback_handle);
    alsa_playback_handle = 0;
  }
}

static int open_sound(void)
{
  return snd_pcm_open (&alsa_playback_handle, alsa_device, SND_PCM_STREAM_PLAYBACK, 0);
}

/* Try to determine whether sound is available.  This is only for GUI purposes.  */
int setup_sound (void)
{
	int err;
	sound_available = 0;

	printf("ALSA lib version: %s\n", SND_LIB_VERSION_STR);
	if ((err = open_sound()) < 0) {
		/* TODO: if the pcm was busy, we should the same as sd-uss does.
		tell the caller that sound is available. in any other
		condition we should just return 0. */
	    	write_log ("ALSA: Can't open audio device: %s\n", snd_strerror (err));
    		return 0;
	}
	snd_pcm_close (alsa_playback_handle);
	alsa_playback_handle = 0;
	sound_available = 1;
	return 1;
}

static int set_hw_params(snd_pcm_t *pcm,
			 snd_pcm_hw_params_t *hw_params,
			 unsigned int *rate,
			 unsigned int channels,
			 snd_pcm_format_t format,
			 unsigned int *buffer_time,
			 snd_pcm_uframes_t *buffer_frames,
			 snd_pcm_uframes_t *period_frames)
{
    int err;
    unsigned int periods = 2;

    err = snd_pcm_hw_params_any (pcm, hw_params);
    if (err < 0)
		return err;

    err = snd_pcm_hw_params_set_access (pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
		return err;

    err = snd_pcm_hw_params_set_format (pcm, hw_params, format);
    if (err < 0)
		return err;

    err = snd_pcm_hw_params_set_channels (pcm, hw_params, channels);
    if (err < 0)
		return err;

    err = snd_pcm_hw_params_set_rate_near (pcm, hw_params, rate, 0);
    if (err < 0)
		return err;

    err = snd_pcm_hw_params_set_buffer_time_near (pcm, hw_params, buffer_time, NULL);
    if (err < 0)
		return err;

    snd_pcm_hw_params_get_buffer_size (hw_params, buffer_frames);
    err = snd_pcm_hw_params_set_periods_near (pcm, hw_params, &periods, NULL);
    if (err < 0)
       return err;
    if (periods == 1)
		return -EINVAL;
    err = snd_pcm_hw_params(pcm, hw_params);
    snd_pcm_hw_params_get_period_size (hw_params, period_frames, NULL);
    return 0;
}

static int set_sw_params(snd_pcm_t *pcm,
			 snd_pcm_sw_params_t *sw_params,
			 snd_pcm_uframes_t buffer_frames,
			 snd_pcm_uframes_t period_frames)
{
    int err;

    err = snd_pcm_sw_params_current (pcm, sw_params);
    if (err < 0)
		return err;

    err = snd_pcm_sw_params_set_start_threshold(pcm, sw_params, (buffer_frames / period_frames) * period_frames);
    if (err < 0)
		return err;

    err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, period_frames);
    if (err < 0)
		return err;

    err = snd_pcm_sw_params_set_stop_threshold(pcm, sw_params, buffer_frames);
    if (err < 0)
		return err;

	/* deprecated. No longer needed? There doesn't seem to be a substitute.
    err = snd_pcm_sw_params_set_xfer_align(pcm, sw_params, 1);
    if (err < 0)
		return err;
	*/

    err = snd_pcm_sw_params(pcm, sw_params);
    if (err < 0)
		return err;
    return 0;
}

int init_sound (void)
{
    unsigned int rate;
    snd_pcm_format_t format;
    unsigned int channels;
    unsigned int dspbits;

    snd_pcm_hw_params_t *hw_params = 0;
    snd_pcm_sw_params_t *sw_params = 0;
    snd_pcm_uframes_t    buffer_frames;
    snd_pcm_uframes_t    period_frames;
    unsigned int         buffer_time;

    snd_output_t *alsa_out;

    int err;

    snd_output_stdio_attach (&alsa_out, stderr, 0);

    dspbits  = 16;
    channels = currprefs.sound_stereo ? 2 : 1;
    rate     = currprefs.sound_freq;

	have_sound = 0;
	alsa_playback_handle = 0;
	printf("ALSA lib version: %s\n", SND_LIB_VERSION_STR);
    if ((err = open_sound()) < 0) {
		write_log ("ALSA: Can't open audio device: %s\n", snd_strerror (err));
		goto nosound;
    }

    /* this is no longer configurable ? - Sven
    buffer_time = currprefs.sound_latency * 1000;
    if (buffer_time < 1000 || buffer_time > 500000)
		buffer_time = 100000;
	*/
	buffer_time = 100000;

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		write_log ("Cannot allocate hardware parameter structure: %s.\n", snd_strerror (err));
		goto nosound;
    }
    if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
		write_log ("Cannot allocate software parameter structure: %s.\n", snd_strerror (err));
		goto nosound;
    }

    switch (dspbits) {
		case 8:
		    format = SND_PCM_FORMAT_S8;
		    break;
		case 16:
		    format = SND_PCM_FORMAT_S16;
		    break;
		default:
		    write_log ("%d-bit samples not supported by UAE.\n", dspbits);
		    goto nosound;
    }

    bytes_per_frame = dspbits / 8 * channels;

    if ((err = set_hw_params (alsa_playback_handle, hw_params, &rate, channels, format, &buffer_time, &buffer_frames, &period_frames)) < 0) {
		write_log ("Cannot set hw parameters: %s.\n", snd_strerror (err));
		goto nosound;
    }

    if ((err = set_sw_params (alsa_playback_handle, sw_params, buffer_frames, period_frames)) < 0) {
		write_log ("Cannot set sw parameters: %s.\n", snd_strerror (err));
		goto nosound;
    }

    paula_sndbufsize = period_frames * bytes_per_frame;
    snd_pcm_hw_params_free (hw_params);
    snd_pcm_sw_params_free (sw_params);

    if ((err = snd_pcm_prepare (alsa_playback_handle)) < 0) {
		write_log ("Cannot prepare audio interface for use: %s.\n", snd_strerror (err));
		goto nosound;
    }

    obtainedfreq = currprefs.sound_freq;

    sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;

    have_sound = 1;
    sound_available = 1;

    write_log ("ALSA: Using device '%s'.\n", alsa_device);
    write_log ("ALSA: Sound configured for %d bits at %d Hz. Buffer length is %u us, period size %lu bytes.\n",
	       dspbits, rate, buffer_time, (unsigned long)(period_frames * bytes_per_frame) );

    if (alsa_verbose)
		snd_pcm_dump (alsa_playback_handle, alsa_out);

    paula_sndbufpt = paula_sndbuffer;

    return 1;

 nosound:
    have_sound = 0;
    if (hw_params)
		snd_pcm_hw_params_free (hw_params);
    if (sw_params)
		snd_pcm_sw_params_free (sw_params);

    close_sound ();
    return 0;
}

void reset_sound (void)
{
}

void pause_sound (void)
{
    if (alsa_playback_handle)
		snd_pcm_drop (alsa_playback_handle);
}

void resume_sound (void)
{
    if (alsa_playback_handle)
		snd_pcm_prepare (alsa_playback_handle);
}

void sound_volume (int dir)
{
}

void restart_sound_buffer (void)
{
}

/*
 * Handle audio specific cfgfile options
 */
void audio_default_options (struct uae_prefs *p)
{
    strncpy (alsa_device, "default", 256);
    alsa_verbose = false;
}

void audio_save_options (struct zfile *f, const struct uae_prefs *p)
{
    cfgfile_write (f, "alsa.device=%s\n", alsa_device);
    cfgfile_write (f, "alsa.verbose=%s\n", alsa_verbose ? "true" : "false");
}

int audio_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return (cfgfile_string (option, value, "device",   alsa_device, 256)
	 || cfgfile_yesno  (option, value, "verbose", &alsa_verbose));
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

