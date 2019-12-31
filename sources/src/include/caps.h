#ifndef UAE_CAPS_H
#define UAE_CAPS_H

void caps_unloadimage (unsigned int drv);
int caps_loadimage (struct zfile *zf, unsigned int drv, unsigned int *num_tracks);
int caps_loadrevolution (uae_u16 *mfmbuf, unsigned int drv, unsigned int track, unsigned int *tracklength);
int caps_loadtrack (uae_u16 *mfmbuf, uae_u16 *tracktiming, unsigned int drv, unsigned int track, unsigned int *tracklength, int *multirev, unsigned int *gapoffset);

#endif // UAE_CAPS_H
