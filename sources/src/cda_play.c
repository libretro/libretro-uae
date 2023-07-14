
#include "sysconfig.h"
#include "sysdeps.h"

#include "cda_play.h"
#include "zfile.h"
#include "zarchive.h"
#include "audio.h"
#include "options.h"

int cda_audio_bufsize;
int cda_audio_volume[2];
bool cda_audio_playing;
bool cda_audio_active;
int cda_audio_buffer_ids[2];
uae_u8 *cda_audio_buffers[2];

int cda_audio_num_sectors;
int cda_audio_sectorsize;

void cda_delete()
{
    if (cda_audio_active) {
        cda_wait(0);
        cda_wait(1);
    }
    for (int i = 0; i < 2; i++) {
        xfree (cda_audio_buffers[i]);
        cda_audio_buffers[i] = NULL;
    }
    cda_audio_bufsize = 0;
}

void cda_new(int num_sectors, int sectorsize, int samplerate)
{
    memset(&cda_audio_buffers, 0, sizeof cda_audio_buffers);

    cda_audio_active = false;
    cda_audio_playing = false;
    cda_audio_volume[0] = cda_audio_volume[1] = 0;

    cda_audio_bufsize = num_sectors * sectorsize;
    cda_audio_sectorsize = sectorsize;
    for (int i = 0; i < 2; i++) {
        cda_audio_buffer_ids[i] = 0;
        cda_audio_buffers[i] = xcalloc (uae_u8, num_sectors * ((cda_audio_bufsize + 4095) & ~4095));
    }
    cda_audio_num_sectors = num_sectors;

    cda_audio_active = true;
    cda_audio_playing = true;
}

void cda_setvolume(int left, int right)
{
    for (int j = 0; j < 2; j++) {
        cda_audio_volume[j] = j == 0 ? left : right;
        cda_audio_volume[j] = sound_cd_volume[j] * cda_audio_volume[j] / 32768;
        if (cda_audio_volume[j])
            cda_audio_volume[j]++;
        cda_audio_volume[j] = cda_audio_volume[j] * (100 - currprefs.sound_volume_master) / 100;
        if (cda_audio_volume[j] >= 32768)
            cda_audio_volume[j] = 32768;
    }
}

bool cda_play(int bufnum)
{
    if (!cda_audio_active)
        return false;

    uae_s16 *p = (uae_s16*)(cda_audio_buffers[bufnum]);
    if (cda_audio_volume[0] != 32768 || cda_audio_volume[1] != 32768) {
        for (int i = 0; i < cda_audio_num_sectors * cda_audio_sectorsize / 4; i++) {
            p[i * 2 + 0] = p[i * 2 + 0] * cda_audio_volume[0] / 32768;
            p[i * 2 + 1] = p[i * 2 + 1] * cda_audio_volume[1] / 32768;
        }
    }

    return true;
}

void cda_wait(int bufnum)
{
    if (!cda_audio_active || !cda_audio_playing)
        return;
}

bool cda_isplaying(int bufnum)
{
    if (!cda_audio_active || !cda_audio_playing)
        return false;
    return true;
}
