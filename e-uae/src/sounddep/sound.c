 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for the Mute sound system
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2003 Richard Drummond
  */



#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "mmemory.h"
#include "custom.h"
#include "audio.h"
#include "gensound.h"

#include "driveclick.h"
#include "sounddep/sound.h"


#define MAX_BSIZE 4096

uae_u16* sndbuffer = NULL;
uae_u16* sndbufpt;
//int sndblocksize;
int sndbufsize;
int sound_initialized = 0;

static int have_sound = 0;
int soundcheck = 0;


unsigned long last_time = 0;
unsigned long now_time;

unsigned long stat_time;
unsigned long stat_count;

//static unsigned long scaled_sample_evtime;

int init_sound (void)
{
    if (sndbuffer != NULL) {
#ifdef DRIVESOUND
	driveclick_init();
#endif	
	return 1;
    }

    int rate = 22500;
    if (sound_initialized) {
	return 1;
    }

    LOG_MSG(("INIT SOUND\n"));
    sndbuffer = (uae_u16*) malloc(MAX_BSIZE);
    if (sndbuffer == NULL) {
	LOG_MSG(("sound init failed" ));
	return 0;
    }
   
    sound_initialized = 1;

    currprefs.sound_freq = rate;
    currprefs.sound_bits = 16;
    currprefs.sound_stereo = 1;

    sndbufsize = MAX_BSIZE;


    sample_handler =  sample16s_handler;
    //scaled_sample_evtime = (unsigned long)(MAXHPOS_PAL * MAXVPOS_PAL * VBLANK_HZ_PAL + rate - 1) / rate;

    obtainedfreq = currprefs.sound_freq;
    sound_available = 1;

    sndbufpt = sndbuffer;
    init_sound_table16();

    LOG_MSG2("bufsize=%i\n", sndbufsize);

#ifdef DRIVESOUND
	driveclick_init();
#endif
    return 1;
}

int setup_sound (void)
{
    LOG_MSG(("setup_sound" ));
    sound_available = 1;
    have_sound = 1;
    return 1;
}

void close_sound (void)
{
}
/*
void update_sound (int freq)
{
    static int lastfreq =0;

    if (freq < 0)
        freq = lastfreq;
    lastfreq = freq;
    if (currprefs.ntscmode)
	scaled_sample_evtime = (unsigned long)(MAXHPOS_NTSC * MAXVPOS_NTSC * freq * CYCLE_UNIT + obtainedfreq - 1) / obtainedfreq;
    else
	scaled_sample_evtime = (unsigned long)(MAXHPOS_PAL * MAXVPOS_PAL * freq * CYCLE_UNIT + obtainedfreq - 1) / obtainedfreq;

}
*/

void reset_sound (void)
{
}

void pause_sound (void)
{
}

void resume_sound (void)
{
//   printf("Sound resume!\n");
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


int audio_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void audio_save_options (FILE *f, const struct uae_prefs *p)
{
}
