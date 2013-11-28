 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for the Mute sound system.
  *
  * Copyright 1997 Bernd Schmidt
  */

#define SOUNDSTUFF 1
extern void restart_sound_buffer (void);

STATIC_INLINE void check_sound_buffers (void)
{
}
STATIC_INLINE void set_sound_buffers (void)
{
}
#define AUDIO_NAME "dummyaudio"

#define PUT_SOUND_BYTE(b) do { ; } while (0)
#define PUT_SOUND_WORD(b) do { ; } while (0)
#define PUT_SOUND_BYTE_LEFT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_LEFT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_LEFT2(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_BYTE_RIGHT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_RIGHT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_RIGHT2(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_WORD_MONO(b) PUT_SOUND_WORD_LEFT(b)

#define SOUND16_BASE_VAL 0
#define SOUND8_BASE_VAL 0

#define DEFAULT_SOUND_MAXB 16384
#define DEFAULT_SOUND_MINB 16384
#define DEFAULT_SOUND_BITS 16
#define DEFAULT_SOUND_FREQ 44100
#define HAVE_STEREO_SUPPORT
                        
#define FILTER_SOUND_OFF 0
#define FILTER_SOUND_EMUL 1
#define FILTER_SOUND_ON 2

#define FILTER_SOUND_TYPE_A500 0
#define FILTER_SOUND_TYPE_A1200 1

