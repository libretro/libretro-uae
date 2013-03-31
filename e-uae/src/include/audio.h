 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Sound emulation stuff
  *
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  */

#define PERIOD_MAX ULONG_MAX

extern void aud0_handler (void);
extern void aud1_handler (void);
extern void aud2_handler (void);
extern void aud3_handler (void);

extern void AUDxDAT (unsigned int nr, uae_u16 value);
extern void AUDxVOL (unsigned int nr, uae_u16 value);
extern void AUDxPER (unsigned int nr, uae_u16 value);
extern void AUDxLCH (unsigned int nr, uae_u16 value);
extern void AUDxLCL (unsigned int nr, uae_u16 value);
extern void AUDxLEN (unsigned int nr, uae_u16 value);

extern void ahi_install (void);
extern void update_audio (void);
extern void schedule_audio (void);
extern void audio_evhandler (void);
extern void audio_hsync (int);
extern void audio_update_adkmasks (void);
extern void audio_update_irq (uae_u16);
extern void update_sound (unsigned int freq);

extern int  audio_setup (void);
extern int  audio_init (void);
extern void audio_close (void);
extern void audio_reset (void);
extern void audio_pause (void);
extern void audio_resume (void);
extern void audio_volume (int);

void switch_audio_interpol (void);

extern int sound_available;
