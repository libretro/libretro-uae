/***********************************************************/
//  BeUAE - The Be Un*x Amiga Emulator
//
//  BeOS port sound routines
//  Using R4 BSoundPlayer class
//
//  (c) 2004-2007 Richard Drummond
//  (c) 2000-2001 Axel D�fler
//  (c) 1999 Be/R4 Sound - Raphael Moll
//  (c) 1998-1999 David Sowsy
//
// History:
//
// -RM/050999 : removed debug code, code cleanup, etc.
// -DS/051399 : adapted code to build under 0.8.8.
//				New Media Kit woes ensued.
// -DS/06xx99 : adapted code to work with the new Game Kit.
// -AD/121300 : supports now different frequencies, bitsrates, and stereo modes
// -AD/121500 : blocks now correctly to synchronize UAE
// -RD/012804 : updated for UAE 0.8.23
/***********************************************************/

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gensound.h"
#include "sounddep/sound.h"
}

#include <be/media/MediaRoster.h>
#include <be/media/SoundPlayer.h>
#include <be/media/MediaDefs.h>

extern "C" {
void finish_sound_buffer (void);
void update_sound (int freq);
int setup_sound (void);
void close_sound (void);
int init_sound (void);
void pause_sound (void);
void resume_sound (void);
void reset_sound (void);
void sound_volume (int dir);
}

uae_u16 *paula_sndbuffer;
uae_u16 *paula_sndbufpt;
int paula_sndbufsize;
BSoundPlayer	*gSoundPlayer;
static int32	 gSoundBufferSize;
static uae_u16	*gDoubleBufferRead;
static uae_u16	*gDoubleBufferWrite;
static uae_u16	*gLastBuffer;
static int32	 gBufferReadPos;

static uae_u16 *buffer = NULL;
static bool sound_ready = false;
static sem_id sound_sync_sem;

void stream_func8  (void *user, void *buffer, size_t size, const media_raw_audio_format &format);
void stream_func16 (void *user, void *buffer, size_t size, const media_raw_audio_format &format);

static int exact_log2 (int v)
{
    int l = 0;
    while ((v >>= 1) != 0)
	l++;
    return l;
}

static int get_nearest_power_of_2 (int v)
{
    int low = 1 << exact_log2 (v);
    int hi  = low << 1;

    if ((v - low) < (hi - v))
	return low;
    else
	return hi;
}

int init_sound (void)
{
    if (gSoundPlayer != NULL)
	return 0;

    media_raw_audio_format audioFormat;

    gSoundBufferSize = currprefs.sound_freq * currprefs.sound_latency *
		       (currprefs.sound_stereo ? 2 : 1) / 1000;
    gSoundBufferSize = get_nearest_power_of_2 (gSoundBufferSize);

    audioFormat.frame_rate    = currprefs.sound_freq;
    audioFormat.channel_count = currprefs.sound_stereo ? 2 : 1;
    audioFormat.format        = media_raw_audio_format::B_AUDIO_FLOAT;
    audioFormat.byte_order    = B_MEDIA_HOST_ENDIAN;
    audioFormat.buffer_size   = gSoundBufferSize * sizeof(float);

    gSoundPlayer = new BSoundPlayer (&audioFormat, "UAE SoundPlayer", stream_func16);
    sound_ready = (gSoundPlayer != NULL);

    if (!currprefs.produce_sound)
	return 3;

    sound_sync_sem  = create_sem (0, "UAE Sound Sync Semaphore");
    gBufferReadPos = 0;
    gDoubleBufferWrite = new uae_u16[2 * gSoundBufferSize];
    gDoubleBufferRead = gDoubleBufferWrite + gSoundBufferSize;

    buffer = gDoubleBufferWrite;
    memset (buffer, 0, 4 * gSoundBufferSize);
    paula_sndbufpt = paula_sndbuffer = buffer;

    paula_sndbufsize = sizeof (uae_u16) * gSoundBufferSize;
    if (currprefs.sound_stereo)
	sample_handler = sample16s_handler;
    else
	sample_handler = sample16_handler;

    sound_available = 1;
    obtainedfreq = currprefs.sound_freq;

    write_log ("BeOS sound driver found and configured at %d Hz, buffer is %d samples (%d ms)\n",
	       currprefs.sound_freq, gSoundBufferSize / audioFormat.channel_count,
	       (gSoundBufferSize / audioFormat.channel_count) * 1000 / currprefs.sound_freq);

    if (gSoundPlayer) {
	gSoundPlayer->Start ();
	gSoundPlayer->SetHasData (true);
	    return 1;
    }
    return 0;
}

int setup_sound (void)
{
    status_t err;
    BMediaRoster *gMediaRoster;
    media_node *outNode;
    outNode = new media_node;
    gMediaRoster = BMediaRoster::Roster (&err);

    if (gMediaRoster && err == B_OK)
	err = gMediaRoster->GetAudioOutput (outNode);

    if ((!gMediaRoster) || (err != B_OK)) {
	write_log ("NO MEDIA ROSTER! The media server "
		   "appears to be dead.\n"
		   "\t-- roster %p -- error %08lx (%ld)\n",
		   gMediaRoster, err, err);

	sound_available = 0;
    } else
	sound_available = 1;

    return sound_available;
}

void close_sound (void)
{
    if (sound_ready) {
	if (gSoundPlayer) {
	    gSoundPlayer->Stop();
	    delete gSoundPlayer;
	    gSoundPlayer = NULL;
	}
	sound_ready = false;
    }

    delete_sem(sound_sync_sem);

    if (gDoubleBufferRead < gDoubleBufferWrite)
	delete[] gDoubleBufferRead;
    else
	delete[] gDoubleBufferWrite;
    gDoubleBufferRead = gDoubleBufferWrite = NULL;
}

void finish_sound_buffer (void)
{
    if (sound_ready && acquire_sem (sound_sync_sem) == B_OK) {
	uae_u16 *p = gDoubleBufferRead;		// swap buffers
	gDoubleBufferRead = gDoubleBufferWrite;
	buffer = gDoubleBufferWrite = p;
    }
    paula_sndbufpt = paula_sndbuffer = buffer;
}

void stream_func16 (void *user, void *buffer, size_t size,const media_raw_audio_format &format)
{
    uae_u16 *buf;
    int32 max_read_sample, avail_sample;

    // since the BSoundPlayer supports only B_AUDIO_FLOAT, it's
    // very unlikely that this will ever happen:
    // if (format.format != media_raw_audio_format::B_AUDIO_FLOAT) return;

    float *dest = (float *)buffer;
    int32 dest_sample = (int32)(size / sizeof (float));
    float *enddest = dest + dest_sample;

    max_read_sample = gSoundBufferSize;
    if (dest_sample < max_read_sample)
	max_read_sample = dest_sample;

    buf = gDoubleBufferRead + gBufferReadPos;
    avail_sample = gSoundBufferSize - gBufferReadPos;
    if (avail_sample < max_read_sample)
	max_read_sample = avail_sample;
    if (max_read_sample)
	gBufferReadPos += max_read_sample;

    const float ratio = 1.f / 32768.f;
    while(max_read_sample--)	// copy the buffer to the stream
    {
	int16 a = (int16)(*(buf++));
	*(dest++) = ((float)a) * ratio;
    }

    // the buffer is no longer needed, so lets release it.
    // if UAE is not fast enough to swap the buffers during play time, the same
    // buffer will be played again
    if (gBufferReadPos == gSoundBufferSize) {
	gBufferReadPos = 0;
	if (gLastBuffer != gDoubleBufferRead) {
	    gLastBuffer = gDoubleBufferRead;
	    release_sem (sound_sync_sem);
	}
    }

    while (dest < enddest)
	*(dest++) = 0.f;
}

void pause_sound (void)
{
    if (gSoundPlayer)
	gSoundPlayer->Stop ();

    return;
}

void resume_sound (void)
{
    if (gSoundPlayer)
	gSoundPlayer->Start ();

    return;
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
