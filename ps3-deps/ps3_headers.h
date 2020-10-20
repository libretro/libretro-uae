#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sys_time.h>
#include <sys/timer.h>
#include <dirent.h>
#include <sys/cdefs.h>
#include <ctype.h>

#ifndef PS3_HEADERS
#define PS3_HEADERS

#define	F_OK		0	/* test for existence of file */
#define	X_OK		0x01	/* test for execute or search permission */
#define	W_OK		0x02	/* test for write permission */
#define	R_OK		0x04	/* test for read permission */
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#define usleep  sys_timer_usleep
#define	getcwd(a,b)	"/dev_hdd0/SSNE10000/USRDIR/"
#define getenv(a)	"/dev_hdd0/SSNE10000/USRDIR/cores/"
#define chdir(a) 0

#undef timezone
struct timezone {
	int tz_minuteswest;     /* minutes west of Greenwich */
	int tz_dsttime;         /* type of DST correction */
};

struct timeb {
	time_t          time;
	unsigned short  millitm;
	short           timezone;
	short           dstflag;
};

#define timezone 0

#endif

int gettimeofday(struct timeval* tv, void* blah);
int fseeko(FILE* stream, off_t pos, int whence);
int ftime(struct timeb* tb);