#ifndef CDA_PLAY_H
#define CDA_PLAY_H

#define CDDA_BUFFERS 14

extern volatile bool cd_audio_mode_changed;

#ifdef __cplusplus
class cda_audio {
private:
    int bufsize;
    int num_sectors;
	int sectorsize;
    int volume[2];
    bool playing;
    bool active;
    int buffer_ids[2];

public:
    uae_u8 *buffers[2];

	cda_audio(int num_sectors, int sectorsize, int samplerate);
	~cda_audio();
	void setvolume(int left, int right);
	bool play(int bufnum);
	void wait(void);
	void wait(int bufnum);
	bool isplaying(int bufnum);
};
#else
typedef struct cda_audio cda_audio;
#endif

extern int cda_audio_bufsize;
extern int cda_audio_volume[2];
extern bool cda_audio_playing;
extern bool cda_audio_active;
extern int cda_audio_buffer_ids[2];
extern uae_u8 *cda_audio_buffers[2];

extern int cda_audio_num_sectors;
extern int cda_audio_sectorsize;
extern int cda_audio_samplerate;
extern bool cda_audio_internalmode;

extern int cda_audio_setvolume;
extern bool cda_audio_play;
extern int cda_audio_wait;
extern bool cda_audio_isplaying;

extern void cda_new(int num_sectors, int sectorsize, int samplerate);
extern void cda_delete(void);
extern void cda_setvolume(int left, int right);
extern bool cda_play(int bufnum);
extern void cda_wait(int bufnum);
extern bool cda_isplaying(int bufnum);

#endif /* CDA_PLAY_H */
