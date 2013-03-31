 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for SDL sound
  *
  * Copyright 1997 Bernd Schmidt
  */

extern int sound_fd;
extern uae_u16 sndbuffer[];
extern uae_u16 *sndbufpt;
extern int sndbufsize;
extern void finish_sound_buffer (void);
extern int init_sound (void);
extern void close_sound (void);
extern int setup_sound (void);
extern void resume_sound (void);
extern void pause_sound (void);
extern void reset_sound (void);

STATIC_INLINE void check_sound_buffers (void)
{
    if ((char *)sndbufpt - (char *)sndbuffer >= sndbufsize) {
	finish_sound_buffer ();
	sndbufpt = sndbuffer;
    }
}

#define AUDIO_NAME "sdl"

#define PUT_SOUND_BYTE(b) do { *(uae_u8 *)sndbufpt = b; sndbufpt = (uae_u16 *)(((uae_u8 *)sndbufpt) + 1); } while (0)
#define PUT_SOUND_WORD(b) do { *(uae_u16 *)sndbufpt = b; sndbufpt = (uae_u16 *)(((uae_u8 *)sndbufpt) + 2); } while (0)
#define PUT_SOUND_BYTE_LEFT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_LEFT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_BYTE_RIGHT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_RIGHT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_MONO(b) PUT_SOUND_WORD_LEFT(b)
#define SOUND16_BASE_VAL 0
#define SOUND8_BASE_VAL 128

#define DEFAULT_SOUND_BITS 16
#define DEFAULT_SOUND_FREQ 44100
#define DEFAULT_SOUND_LATENCY 100
#define HAVE_STEREO_SUPPORT
#define HAVE_8BIT_AUDIO_SUPPORT
