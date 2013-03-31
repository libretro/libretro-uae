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

#include "options.h"
#include "gensound.h"
#include "sounddep/sound.h"

#include <alsa/asoundlib.h>

char alsa_device[256];
int  alsa_verbose;

static int have_sound = 0;

uae_u16 sndbuffer[44100];
uae_u16 *sndbufpt;
int sndbufsize;

snd_pcm_t *alsa_playback_handle = 0;
int bytes_per_frame;


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
  if ((err = open_sound()) < 0) {
    /* TODO: if the pcm was busy, we should the same as sd-uss does.
       tell the caller that sound is available. in any other
       condition we should just return 0. */
    write_log ("Cannot open audio device: %s.\n", snd_strerror (err));
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
    err = snd_pcm_sw_params_set_xfer_align(pcm, sw_params, 1);
    if (err < 0)
	return err;
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

    dspbits  = currprefs.sound_bits;
    channels = currprefs.sound_stereo ? 2 : 1;
    rate     = currprefs.sound_freq;

    have_sound = 0;
    alsa_playback_handle = 0;
    if ((err = open_sound()) < 0) {
	write_log ("Cannot open audio device: %s\n", snd_strerror (err));
	goto nosound;
    }

    buffer_time = currprefs.sound_latency * 1000;
    if (buffer_time < 1000 || buffer_time > 500000)
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

    sndbufsize = period_frames * bytes_per_frame;
    snd_pcm_hw_params_free (hw_params);
    snd_pcm_sw_params_free (sw_params);

    if ((err = snd_pcm_prepare (alsa_playback_handle)) < 0) {
	write_log ("Cannot prepare audio interface for use: %s.\n", snd_strerror (err));
	goto nosound;
    }

    obtainedfreq = currprefs.sound_freq;

    if (dspbits == 16) {
	init_sound_table16 ();
	sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;
    } else {
	init_sound_table8 ();
	sample_handler = currprefs.sound_stereo ? sample8s_handler : sample8_handler;
    }
    have_sound = 1;
    sound_available = 1;

    write_log ("ALSA: Using device '%s'.\n", alsa_device);
    write_log ("ALSA: Sound configured for %d bits at %d Hz. Buffer length is %u us, period size %d bytes.\n",
	       dspbits, rate, buffer_time, period_frames * bytes_per_frame);

    if (alsa_verbose)
	snd_pcm_dump (alsa_playback_handle, alsa_out);

    sndbufpt = sndbuffer;

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

/*
 * Handle audio specific cfgfile options
 */
void audio_default_options (struct uae_prefs *p)
{
    strncpy (alsa_device, "default", 256);
    alsa_verbose = 0;
}

void audio_save_options (FILE *f, const struct uae_prefs *p)
{
    cfgfile_write (f, "alsa.device=%s\n", alsa_device);
    cfgfile_write (f, "alsa.verbose=%s\n", alsa_verbose ? "true" : "false");
}

int audio_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return (cfgfile_string (option, value, "device",   alsa_device, 256)
	 || cfgfile_yesno  (option, value, "verbose", &alsa_verbose));
}
