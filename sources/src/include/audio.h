/*
 * UAE - The Un*x Amiga Emulator
 *
 * Sound emulation stuff
 *
 * Copyright 1995, 1996, 1997 Bernd Schmidt
 */

#ifndef UAE_AUDIO_H
#define UAE_AUDIO_H

#define PERIOD_MAX ULONG_MAX
#define MAX_EV ~0u

extern void pause_sound (void);
extern void resume_sound (void);

extern void aud0_handler (void);
extern void aud1_handler (void);
extern void aud2_handler (void);
extern void aud3_handler (void);

extern void AUDxDAT (int nr, uae_u16 value);
extern void AUDxDAT_addr (int nr, uae_u16 value, uaecptr addr);
extern void AUDxVOL (int nr, uae_u16 value);
extern void AUDxPER (int nr, uae_u16 value);
extern void AUDxLCH (int nr, uae_u16 value);
extern void AUDxLCL (int nr, uae_u16 value);
extern void AUDxLEN (int nr, uae_u16 value);

extern uae_u16 audio_dmal (void);
extern void audio_state_machine (void);
extern uaecptr audio_getpt (int nr, bool reset);

extern int init_audio (void);
extern void ahi_install (void);
extern void audio_reset (void);
extern void update_audio (void);
extern void audio_evhandler (void);
extern void audio_hsync (void);
extern void audio_update_adkmasks (void);
extern void update_sound (double freq, int longframe, int linetoggle);
//extern void update_sound (double clk);
extern void update_cda_sound (double clk);
extern void led_filter_audio (void);
extern void set_audio (void);
extern int audio_activate (void);
extern void audio_deactivate (void);
extern void audio_vsync (void);

void switch_audio_interpol (void);
extern int sound_available;

extern void audio_sampleripper(int);
extern int sampleripper_enabled;
extern void write_wavheader (struct zfile *wavfile, uae_u32 size, uae_u32 freq);

extern void audio_update_sndboard(unsigned int);
extern void audio_enable_sndboard(bool);
extern void audio_state_sndboard(int);
extern void audio_state_sndboard_state(int, int, unsigned int);

typedef void (*CDA_CALLBACK)(int, void*);
typedef bool(*SOUND_STREAM_CALLBACK)(int, void*);

extern int audio_enable_stream(bool, int, int, SOUND_STREAM_CALLBACK, void*);
extern void audio_state_stream_state(int, int*, int, unsigned int);

struct cd_audio_state
{
	uae_s16 *cda_bufptr;
	int cda_length, cda_userdata;
	CDA_CALLBACK cda_next_cd_audio_buffer_callback;
	void *cb_data;
	int cda_volume[2];
	int cda_streamid;// = -1;
};

extern void audio_cda_new_buffer(struct cd_audio_state *cas, uae_s16 *buffer, int length, int userdata, CDA_CALLBACK next_cd_audio_buffer_callback, void *cb_data);
extern void audio_cda_volume(struct cd_audio_state *cas, int left, int right);

extern int sound_cd_volume[2];
extern int sound_paula_volume[2];

#define AUDIO_CHANNEL_MAX_STREAM_CH 8
#define AUDIO_CHANNEL_STREAMS 9

#define AUDIO_CHANNELS_PAULA 4

enum {
    SND_MONO,
    SND_STEREO,
    SND_4CH_CLONEDSTEREO,
    SND_4CH,
    SND_6CH_CLONEDSTEREO,
    SND_6CH,
    SND_NONE
};

STATIC_INLINE int get_audio_stereomode (int channels)
{
    switch (channels)
    {
	case 1:
		return SND_MONO;
	case 2:
		return SND_STEREO;
	case 4:
		return SND_4CH;
	case 6:
		return SND_6CH;
    }
    return SND_STEREO;
}
STATIC_INLINE int get_audio_nativechannels (int stereomode)
{
    int ch[] = { 1, 2, 4, 4, 6, 6, 0 };
    return ch[stereomode];
}
STATIC_INLINE int get_audio_amigachannels (int stereomode)
{
    int ch[] = { 1, 2, 2, 4, 2, 4, 0 };
    return ch[stereomode];
}
STATIC_INLINE int get_audio_ismono (int stereomode)
{
    return stereomode == 0;
}

#define SOUND_MAX_DELAY_BUFFER 1024
#define SOUND_MAX_LOG_DELAY 10
#define MIXED_STEREO_MAX 16
#define MIXED_STEREO_SCALE 32

#endif /* UAE_AUDIO_H */
