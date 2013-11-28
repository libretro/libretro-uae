 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Solaris sound
  *
  * Copyright 1996, 1997 Manfred Thole
  * Copyright 2007       Richard Drummond
  */

#define SOUNDSTUFF 1

extern uae_u16 paula_sndbuffer[];
extern uae_u16 *paula_sndbufpt;
extern int sound_fd;
extern int paula_sndbufsize;

STATIC_INLINE void flush_sound_buffer (void)
{
    write (sound_fd, paula_sndbuffer, paula_sndbufsize);
    paula_sndbufpt = paula_sndbuffer;
}

STATIC_INLINE void check_sound_buffers (void)
{
    if ((char *)paula_sndbufpt - (char *)paula_sndbuffer >= paula_sndbufsize) {
	flush_sound_buffer ();
    }
}

#define AUDIO_NAME "solaris"

#define PUT_SOUND_BYTE(b) do { *(uae_u8 *)paula_sndbufpt = b; paula_sndbufpt = (uae_u16 *)(((uae_u8 *)paula_sndbufpt) + 1); } while (0)
#define PUT_SOUND_WORD(b) do { *(uae_u16 *)paula_sndbufpt = b; paula_sndbufpt = (uae_u16 *)(((uae_u8 *)paula_sndbufpt) + 2); } while (0)

#define PUT_SOUND_WORD_MONO(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_LEFT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_RIGHT(b) PUT_SOUND_WORD(b)

#define SOUND16_BASE_VAL 0
#define DEFAULT_SOUND_FREQ 44100
#define DEFAULT_SOUND_LATENCY 100
