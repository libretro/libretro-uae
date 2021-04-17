#include "ps3_headers.h"

#ifndef __PSL1GHT__
int fseeko(FILE* stream, off_t pos, int whence)
{
	return fseek(stream, (long)pos, whence);
}

off_t ftello( FILE* stream )
{
	return (off_t)ftell(stream);
}

INLINE int gettimeofday(struct timeval* tv, void* blah)
{
	int64_t time = sys_time_get_system_time();

	tv->tv_sec = time / 1000000;
	tv->tv_usec = time - (tv->tv_sec * 1000000);  // implicit rounding will take care of this for us
	return 0;
}
#endif
