#ifndef CDA_PLAY_H
#define CDA_PLAY_H

extern volatile bool cd_audio_mode_changed;

#ifdef __cplusplus
class cda_audio {
private:
    int bufsize;
    int volume[2];
    bool playing;
    bool active;
    int buffer_ids[2];

public:
    uae_u8 *buffers[2];
    int num_sectors;
	int sectorsize;

	cda_audio(int num_sectors, int sectorsize, int samplerate, bool internalmode);
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

int cda_audio_bufsize;
int cda_audio_volume[2];
bool cda_audio_playing;
bool cda_audio_active;
int cda_audio_buffer_ids[2];
uae_u8 *cda_audio_buffers[2];

int cda_audio_num_sectors;
int cda_audio_sectorsize;
int cda_audio_samplerate;
bool cda_audio_internalmode;

int cda_audio_setvolume;
bool cda_audio_play;
int cda_audio_wait;
bool cda_audio_isplaying;

void cda_new(int num_sectors, int sectorsize, int samplerate, bool internalmode);
void cda_delete();
void cda_setvolume(int left, int right);
bool cda_play(int bufnum);
void cda_wait(int bufnum);
bool cda_isplaying(int bufnum);

#endif /* CDA_PLAY_H */
