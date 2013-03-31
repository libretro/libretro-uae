/* posixemu prototypes */
#ifndef __POSIXEMU_H__
#define __POSIXEMU_H__

void fname_atow (const char *src, char *dst, int size);
void fname_wtoa (unsigned char *ptr);
int w32fopendel(char *name, char *mode, int delflag);

#ifndef HAVE_GETTIMEOFDAY
void gettimeofday (struct timeval *tv, void *blah);
#endif

#ifndef HAVE_TRUNCATE
int truncate (const char *name, long int len);
#endif

#endif
