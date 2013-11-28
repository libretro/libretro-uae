 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Amiga audio.device and AHI.device sound.
  *
  * Copyright 1996, 1997, 1998 Samuel Devulder, Holger Jakob (AHI)
  */

#define SOUNDSTUFF 1

#include <exec/memory_uae.h>
#include <exec/devices.h>
#include <exec/io.h>

#include <graphics/gfxbase.h>
#include <devices/timer.h>
#include <devices/ahi.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>

extern struct AHIRequest *AHIio[];
extern struct AHIRequest *linkio;

extern unsigned char *buffers[2];
extern uae_u16 *paula_sndbuffer, *paula_sndbufpt;
extern int bufidx;
extern int paula_sndbufsize;

STATIC_INLINE void flush_sound_buffer (void)
{
	void *tmp;

	AHIio[bufidx]->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
	AHIio[bufidx]->ahir_Std.io_Command  = CMD_WRITE;
	AHIio[bufidx]->ahir_Std.io_Data     = buffers[bufidx];
	AHIio[bufidx]->ahir_Std.io_Length   = paula_sndbufsize;
	AHIio[bufidx]->ahir_Std.io_Offset   = 0;
	AHIio[bufidx]->ahir_Frequency       = currprefs.sound_freq;
	if (currprefs.sound_stereo)
		AHIio[bufidx]->ahir_Type    = AHIST_S16S;
	else
		AHIio[bufidx]->ahir_Type    = AHIST_M16S;
	AHIio[bufidx]->ahir_Volume          = 0x10000;          /* Full volume */
	AHIio[bufidx]->ahir_Position        = 0x8000;           /* Centered */
	AHIio[bufidx]->ahir_Link            = linkio;
	SendIO ((struct IORequest *) AHIio[bufidx]);

	if (linkio)
	    WaitIO ((struct IORequest *) linkio);
	linkio = AHIio[bufidx];
	/* double buffering */
	bufidx = 1 - bufidx;
	paula_sndbuffer = paula_sndbufpt = (uae_u16*) buffers[bufidx];
}

STATIC_INLINE void check_sound_buffers (void)
{
    if ((char *)paula_sndbufpt - (char *)paula_sndbuffer >= paula_sndbufsize) {
	flush_sound_buffer ();
    }
}

#define AUDIO_NAME "amiga"

#define PUT_SOUND_BYTE(b) do { *(uae_u8 *) paula_sndbufpt = b; paula_sndbufpt = (uae_u16 *)(((uae_u8 *)paula_sndbufpt) + 1); } while (0)
#define PUT_SOUND_WORD(b) do { *(uae_u16 *)paula_sndbufpt = b; paula_sndbufpt = (uae_u16 *)(((uae_u8 *)paula_sndbufpt) + 2); } while (0)

#define PUT_SOUND_WORD_MONO(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_BYTE_LEFT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_LEFT(b) PUT_SOUND_WORD(b)
#define PUT_SOUND_BYTE_RIGHT(b) PUT_SOUND_BYTE(b)
#define PUT_SOUND_WORD_RIGHT(b) PUT_SOUND_WORD(b)

#define SOUND16_BASE_VAL 0
#define DEFAULT_SOUND_FREQ 11025
#define DEFAULT_SOUND_LATENCY 100
#define HAVE_STEREO_SUPPORT
