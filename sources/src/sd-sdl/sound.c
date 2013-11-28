/*
 * UAE - The Un*x Amiga Emulator
 *
 * Support for SDL sound
 *
 * Copyright 1997 Bernd Schmidt
 * Copyright 2003-2006 Richard Drummond
 * Copyright 2009-2010 Mustafa TUFAN
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "audio.h"
#include "memory_uae.h"
#include "events.h"
#include "custom.h"
#include "gui.h"
#include "gensound.h"
#include "driveclick.h"
#include "sounddep/sound.h"
#include "threaddep/thread.h"
#include <SDL_audio.h>

int have_sound = 0;
static int statuscnt;

uae_u16 paula_sndbuffer[44100];
uae_u16 *paula_sndbufpt;
int paula_sndbufsize;
static SDL_AudioSpec spec;

static smp_comm_pipe to_sound_pipe;
static uae_sem_t data_available_sem, callback_done_sem, sound_init_sem;

static struct sound_data sdpaula;
static struct sound_data *sdp = &sdpaula;

static int in_callback, closing_sound;

static void clearbuffer (void)
{
    memset (paula_sndbuffer, 0, sizeof (paula_sndbuffer));
}

/* This shouldn't be necessary . . . */
static void dummy_callback (void *userdata, Uint8 *stream, int len)
{
	return;
}

static void sound_callback (void *userdata, Uint8 *stream, int len)
{
	if (closing_sound)
		return;
	in_callback = 1;

	/* Wait for data to finish.  */
	uae_sem_wait (&data_available_sem);

	if (!closing_sound) {
		memcpy (stream, paula_sndbuffer, paula_sndbufsize);

		/* Notify writer that we're done.  */
		uae_sem_post (&callback_done_sem);
	}
	in_callback = 0;
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
	uae_sem_post (&data_available_sem);
	uae_sem_wait (&callback_done_sem);
}

/* Try to determine whether sound is available. */
int setup_sound (void)
{
	int success = 0;

	if (SDL_InitSubSystem (SDL_INIT_AUDIO) == 0) {
		spec.freq = currprefs.sound_freq;
		spec.format = AUDIO_S16SYS;
		spec.channels = currprefs.sound_stereo ? 2 : 1;
		spec.callback = dummy_callback;
		spec.samples  = 0; //spec.freq; //* currprefs.sound_latency / 1000;
		spec.callback = sound_callback;
		spec.userdata = 0;

		if (SDL_OpenAudio (&spec, 0) < 0) {
		    write_log ("SDL: Couldn't open audio: %s\n", SDL_GetError());
		    SDL_QuitSubSystem (SDL_INIT_AUDIO);
		} else {
		    success = 1;
		    SDL_CloseAudio ();
		}
	}

	sound_available = success;
	return sound_available;
}

static int open_sound (void)
{
	if (!currprefs.produce_sound)
		return 0;
	config_changed = 1;

	spec.freq = currprefs.sound_freq;
	spec.format = AUDIO_S16SYS;
	spec.channels = currprefs.sound_stereo ? 2 : 1;
	spec.samples  = 0; //spec.freq; //* currprefs.sound_latency / 1000;
	spec.callback = sound_callback;
	spec.userdata = 0;

	clearbuffer();
	if (SDL_OpenAudio (&spec, NULL) < 0) {
		write_log ("SDL: Couldn't open audio: %s\n", SDL_GetError());
		return 0;
	}

	sample_handler = currprefs.sound_stereo ? sample16s_handler : sample16_handler;

	obtainedfreq = currprefs.sound_freq;
	write_log ("SDL: sound driver found and configured at %d Hz, buffer is %d ms (%d bytes).\n", spec.freq, spec.samples * 1000 / spec.freq, paula_sndbufsize);

	have_sound = 1;
	sound_available = 1;
	update_sound (fake_vblank_hz, 1, currprefs.ntscmode);
	paula_sndbufsize = spec.samples * 2 * spec.channels;
	paula_sndbufpt = paula_sndbuffer;
#ifdef DRIVESOUND
	driveclick_init();
#endif

	return 1;
}

static void *sound_thread (void *dummy)
{
	for (;;) {
		int cmd = read_comm_pipe_int_blocking (&to_sound_pipe);

		switch (cmd) {
			case 0:
				open_sound ();
				uae_sem_post (&sound_init_sem);
				break;
			case 1:
				uae_sem_post (&sound_init_sem);
				return 0;
			}
	}
}

/* We need a thread for this, since communication between finish_sound_buffer
 * and the callback works through semaphores.  In theory, this is unnecessary,
 * since SDL uses a sound thread internally, and the callback runs in its
 * context.  But we don't want to depend on SDL's internals too much.  */
static void init_sound_thread (void)
{
	uae_thread_id tid;

	init_comm_pipe (&to_sound_pipe, 20, 1);
	uae_sem_init (&data_available_sem, 0, 0);
	uae_sem_init (&callback_done_sem, 0, 0);
	uae_sem_init (&sound_init_sem, 0, 0);
	uae_start_thread ("Sound", sound_thread, NULL, &tid);
}

void close_sound (void)
{
	config_changed = 1;
	gui_data.sndbuf = 0;
	gui_data.sndbuf_status = 3;
	if (!have_sound)
		return;

	SDL_PauseAudio (1);
	clearbuffer();
	if (in_callback) {
		closing_sound = 1;
		uae_sem_post (&data_available_sem);
	}

	write_comm_pipe_int (&to_sound_pipe, 1, 1);
	uae_sem_wait (&sound_init_sem);
	SDL_CloseAudio ();
	uae_sem_destroy (&data_available_sem);
	uae_sem_destroy (&sound_init_sem);
	uae_sem_destroy (&callback_done_sem);
	have_sound = 0;
}

int init_sound (void)
{
	gui_data.sndbuf_status = 3;
	gui_data.sndbuf = 0;
	if (!sound_available)
		return 0;
	if (currprefs.produce_sound <= 1)
		return 0;
	if (have_sound)
		return 1;

	in_callback = 0;
	closing_sound = 0;

	init_sound_thread ();
	write_comm_pipe_int (&to_sound_pipe, 0, 1);
	uae_sem_wait (&sound_init_sem);
	SDL_PauseAudio (0);
#ifdef DRIVESOUND
	driveclick_reset ();
#endif
	return have_sound;
}

void pause_sound (void)
{
	if (!have_sound)
		return;
	SDL_PauseAudio (1);
}

void resume_sound (void)
{
	if (!have_sound)
		return;
	clearbuffer();
	SDL_PauseAudio (0);
}

void reset_sound (void)
{
	clearbuffer();
	return;
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
}

void audio_save_options (FILE *f, const struct uae_prefs *p)
{
}

int audio_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
	return 0;
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
