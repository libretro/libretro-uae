#include "zfile.h"
#include "fsdb.h"

#include <sys/time.h>

#ifdef __CELLOS_LV2__
#include "ps3_headers.h"
#endif

#ifdef VITA
#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>
#define rmdir(name) sceIoRmdir(name)
#endif

#if !defined(_WIN32) && !defined(__CELLOS_LV2__)
#include <sys/timeb.h>
#endif

typedef int BOOL;
extern int log_filesys;

#ifndef _WIN32

#define INVALID_HANDLE_VALUE		((HANDLE)~0U)
#define INVALID_FILE_ATTRIBUTES		((DWORD) -1)
#define ERROR_ACCESS_DENIED		5
#define INVALID_SET_FILE_POINTER	((DWORD)-1)
#define NO_ERROR			0

#endif

#define FILE_BEGIN	0
#define FILE_CURRENT	1
#define FILE_END	2

#define FILE_FLAG_WRITE_THROUGH		0x80000000
#ifndef _WIN32
#define FILE_FLAG_OVERLAPPED		0x40000000
#define FILE_FLAG_NO_BUFFERING		0x20000000
#define FILE_FLAG_RANDOM_ACCESS		0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN	0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE	0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS	0x02000000
#define FILE_FLAG_POSIX_SEMANTICS	0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT	0x00200000
#define FILE_FLAG_OPEN_NO_RECALL	0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE	0x00080000
#endif

#define CREATE_NEW		1
#define CREATE_ALWAYS		2
#define OPEN_EXISTING		3
#define OPEN_ALWAYS		4
#define TRUNCATE_EXISTING	5

#define FILE_ATTRIBUTE_NORMAL		0x00000080
#define FILE_ATTRIBUTE_READONLY		0x00000001
#define FILE_ATTRIBUTE_HIDDEN		0x00000002
#define FILE_ATTRIBUTE_SYSTEM		0x00000004
#define FILE_ATTRIBUTE_DIRECTORY	0x00000010

#ifndef _WIN32
#define FILE_READ_DATA		0x0001
#define FILE_WRITE_DATA		0x0002
#define FILE_APPEND_DATA	0x0004
#define GENERIC_READ		FILE_READ_DATA
#define GENERIC_WRITE		FILE_WRITE_DATA
#endif
#define FILE_SHARE_READ		0x00000001
#define FILE_SHARE_WRITE	0x00000002
#define FILE_SHARE_DELETE	0x00000004

#ifndef _WIN32
typedef struct {
	DWORD LowPart;
	int32_t HighPart;
	LONGLONG QuadPart;
} LARGE_INTEGER;
#endif

/*#ifdef _WIN32
 #define S_IRGRP 00040
 #define S_IWGRP 00020
 #define S_IROTH 00004
 #define S_IWOTH 00002
#endif*/

int my_errno = 0;

bool my_stat (const TCHAR *name, struct mystat *ms) {
    struct stat sonuc;
    if (stat(name, &sonuc) == -1) {
        write_log("my_stat: stat on file %s failed\n", name);
        return false;
    }
    if (log_filesys)
        write_log("fs_stat returned size %9jd: %s\n", sonuc.st_size, name);
    ms->size = sonuc.st_size;
    ms->mode = 0;
    if (sonuc.st_mode & S_IRUSR) {
        ms->mode |= FILEFLAG_READ;
    }
    if (sonuc.st_mode & S_IWUSR) {
        ms->mode |= FILEFLAG_WRITE;
    }
    ms->mtime.tv_sec = sonuc.st_mtime;
    ms->mtime.tv_usec = 0;
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
    ms->st_blocks = sonuc.blocks;
#endif
    return true;
}

static int setfiletime (const TCHAR *name, int days, int minute, int tick, int tolocal)
{
	return 0;
}

#if defined(__CELLOS_LV2__) || defined(_WIN32) || defined(WIIU) || defined(VITA)
//#warning LSTAT STAT
#define lstat stat
#endif

bool my_utime (const TCHAR *name, struct mytimeval *tv)
{
        int tolocal;
        int days, mins, ticks;
        struct mytimeval tv2;
#if !defined(__CELLOS_LV2__) && !defined(WIIU) && !defined(__SWITCH__) && !defined(VITA)
        if (!tv) {
                struct timeb time;
                ftime (&time);
                tv2.tv_sec = time.time;
                tv2.tv_usec = time.millitm * 1000;
                tolocal = 0;
        } else
#else
                tv2.tv_sec = 1;
                tv2.tv_usec = 1000;
                tolocal = 0;
#endif
	    {
                tv2.tv_sec = tv->tv_sec;
                tv2.tv_usec = tv->tv_usec;
                tolocal = 1;
        }
        timeval_to_amiga (&tv2, &days, &mins, &ticks);
        if (setfiletime (name, days, mins, ticks, tolocal))
                return true;

        return false;
}

int my_existsfile (const char *name)
{
#ifdef WIIU
    FILE * file = fopen(name, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
#else
    struct stat sonuc;
    if (lstat (name, &sonuc) == -1) {
        return 0;
    } else {
        if (!S_ISDIR(sonuc.st_mode))
            return 1;
    }
    return 0;
#endif
}

int my_existsdir (const char *name)
{
#ifdef WIIU
    DIR* dir = opendir(name);
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
        return 1;
    }
    else 
    {
        /* Directory does not exist. */
        return 0;
    }
#else
    struct stat sonuc;

    if (lstat (name, &sonuc) == -1) {
        return 0;
    } else {
        if (S_ISDIR(sonuc.st_mode))
            return 1;
    }
    return 0;
#endif
}

int my_getvolumeinfo (const char *root)
{
    struct stat sonuc;
    int ret = 0;

    if (lstat (root, &sonuc) == -1)
        return -1;
    if (!S_ISDIR(sonuc.st_mode))
        return -1;

    ret |= MYVOLUMEINFO_STREAMS;
    return ret;
}

FILE *my_opentext (const TCHAR *name)
{
    FILE *f;
    uae_u8 tmp[4];
    int v;

    f = _tfopen (name, "rb");
    if (!f)
        return NULL;
    v = fread (tmp, 1, 4, f);
    fclose (f);
    if (v == 4) {
        if (tmp[0] == 0xef && tmp[1] == 0xbb && tmp[2] == 0xbf)
            return _tfopen (name, "r, ccs=UTF-8");
        if (tmp[0] == 0xff && tmp[1] == 0xfe)
            return _tfopen (name, "r, ccs=UTF-16LE");
    }
    return _tfopen (name, "r");
}

struct my_opendir_s *my_opendir (const TCHAR *name, const TCHAR *mask)
{
    struct my_opendir_s *mod;
    mod = xmalloc (struct my_opendir_s, 1);
    if (!mod)
        return NULL;

    DIR* dir = opendir(name);
    if (!dir) {
        write_log("my_opendir '%s' failed\n", name);
        return NULL;
    } else if (log_filesys) {
        write_log("my_opendir '%s'\n", name);
    }

    mod->dh = dir;
    return mod;
}

void my_closedir (struct my_opendir_s *mod)
{
    if (mod)
    {
        closedir(mod->dh);
        free(mod);
    }
}

int my_readdir(struct my_opendir_s* mod, TCHAR* name)
{
    mod->dp = readdir(mod->dh);
    if (!mod->dp)
        return 0;
    _tcscpy (name, mod->dp->d_name);
    if (log_filesys) {
        write_log("my_readdir => '%s'\n", name);
    }
    return 1;
}

int my_rmdir (const TCHAR *name)
{
    struct my_opendir_s *od;
    int cnt;
    TCHAR tname[MAX_DPATH];
    memset(tname, 0, sizeof(TCHAR) * MAX_DPATH);

    /* SHFileOperation() ignores FOF_NORECURSION when deleting directories.. */
    od = my_opendir (name, 0);
    if (!od) {
        return -1;
    }
    cnt = 0;
    while (my_readdir (od, tname)) {
        if (!_tcscmp (tname, _T(".")) || !_tcscmp (tname, _T("..")))
            continue;
        cnt++;
        break;
    }
    my_closedir (od);
    if (cnt > 0) {
        return -1;
    }

    return rmdir (name);
}

/* "move to Recycle Bin" (if enabled) -version of DeleteFile() */
int my_unlink (const TCHAR *name)
{
	return unlink (name);
}

int my_rename (const TCHAR *oldname, const TCHAR *newname)
{
	return rename(oldname, newname);
}

void my_close(struct my_openfile_s* mos)
{
    if (log_filesys)
        write_log("my_close (%s)\n", mos->path);
    errno = 0;
    free(mos->path);
#ifdef WIN32
    int result = _close(mos->fd);
#else
    int result = close(mos->fd);
#endif
    my_errno = errno;
    if (result != 0) {
        write_log("error closing file\n");
#ifdef WIN32
        write_log("Windows error code %lu\n", GetLastError());
#endif
    }
    free(mos);
}

uae_s64 my_lseek (struct my_openfile_s *mos, uae_s64 offset, int whence)
{
	if (log_filesys)
	    write_log("my_lseek %s %lld %d\n", mos->path, offset, whence);
	off_t result = lseek(mos->fd, offset, whence);
	if (log_filesys)
	    write_log("lseek result %jd\n", result);
	return result;
}

struct my_openfile_s *my_open (const TCHAR *name, int flags)
{
    if (log_filesys)
        write_log("my_open %s flags=%d\n", name, flags);

    int open_flags = O_BINARY;
    if (flags & O_TRUNC) {
        open_flags = open_flags | O_TRUNC; //write_log("  O_TRUNC\n");
    }
    if (flags & O_CREAT) {
        open_flags = open_flags | O_CREAT; //write_log("  O_CREAT\n");
    }
    if (flags & O_RDWR) {
        open_flags = open_flags | O_RDWR; //write_log("  O_RDRW\n");
    }
    else if (flags & O_RDONLY) {
        open_flags = open_flags | O_RDONLY; //write_log("  O_RDONLY\n");
    }
    else if (flags & O_WRONLY) {
        open_flags = open_flags | O_WRONLY; //write_log("  O_WRONLY\n");
    }

    int file = open(name, open_flags, 0644);
	struct my_openfile_s *mos;
	mos = xmalloc (struct my_openfile_s, 1);
	if (!mos)
		return NULL;

	mos->fd = file;
	mos->path = my_strdup(name);
	my_errno = 0;
	return mos;
}

int my_truncate(const TCHAR *name, uae_u64 len) {
    int int_len = (int) len;
    if (log_filesys)
        write_log("my_truncate %s len = %d\n", name, int_len);
    struct my_openfile_s *mos = my_open(name, O_WRONLY);
    if (mos == NULL) {
        my_errno = errno;
        write_log("WARNING: opening file for truncation failed\n");
        return -1;
    }
#ifdef WINDOWS
    int result = _chsize(mos->fd, int_len);
#else
    int result = ftruncate(mos->fd, int_len);
#endif
    my_close(mos);
    my_errno = 0;
    return result;
}

uae_s64 my_fsize(struct my_openfile_s* mos) {
    struct stat sonuc;
    if (fstat(mos->fd, &sonuc) == -1) {
        write_log("my_fsize: fstat on file %s failed\n", mos->path);
        return -1;
    }
    return sonuc.st_size;
}

unsigned int my_read(struct my_openfile_s *mos, void *b, unsigned int size) {
    ssize_t bytes_read = read(mos->fd, b, size);
    if (bytes_read == -1) {
        my_errno = errno;
        write_log("WARNING: my_read failed (-1)\n");
        // can only return 0 since return type is unsigned...
        return 0;
    }
    my_errno = 0;
    if (log_filesys)
        write_log("my_read size=%d => %zd\n", size, bytes_read);
    return (unsigned int) bytes_read;
}

unsigned int my_write(struct my_openfile_s *mos, void *b, unsigned int size) {
    ssize_t bytes_written = write(mos->fd, b, size);
    if (bytes_written == -1) {
        my_errno = errno;
        write_log("WARNING: my_write failed (-1) fd=%d buffer=%p size=%d\n",
                mos->fd, b, size);
        // can only return 0 since return type is unsigned...
        write_log("errno %d\n", my_errno);
        write_log("  mos %p -> fd=%d\n", mos, mos->fd);
        //write_log("  GetLastError = %d\n", GetLastError());
        return 0;
    }
    my_errno = 0;
    if (log_filesys)
        write_log("my_write fd=%d buffer=%p size=%d => %zd\n", mos->fd, b, size, bytes_written);
    return (unsigned int) bytes_written;
}

void my_canonicalize_path(const TCHAR *path, TCHAR *out, int size)
{
	_tcsncpy (out, path, size);
	out[size - 1] = 0;
	return;
}

int my_issamevolume(const TCHAR *path1, const TCHAR *path2, TCHAR *path)
{
	TCHAR p1[MAX_DPATH];
	TCHAR p2[MAX_DPATH];
	unsigned int len, cnt;

	my_canonicalize_path(path1, p1, sizeof p1 / sizeof (TCHAR));
	my_canonicalize_path(path2, p2, sizeof p2 / sizeof (TCHAR));
	len = _tcslen (p1);
	if (len > _tcslen (p2))
		len = _tcslen (p2);
	if (_tcsnicmp (p1, p2, len))
		return 0;
	_tcscpy (path, p2 + len);
	cnt = 0;
	for (unsigned int i = 0; i < _tcslen (path); i++) {
		if (path[i] == '\\' || path[i] == '/') {
			path[i] = '/';
			cnt++;
		}
	}
	if (log_filesys)
	    write_log (_T("'%s' (%s) matched with '%s' (%s), extra = '%s'\n"), path1, p1, path2, p2, path);
	return cnt;
}

int my_setcurrentdir (const TCHAR *curdir, TCHAR *oldcur)
{
#ifdef VITA
/// FIXME: do not depend on current working dir on Vita
    return 0;
#else
    int ret = 0;
    if (oldcur)
        ret = getcwd (oldcur, MAX_DPATH) ? 0 : 1;
    if (curdir) {
        const TCHAR *namep;
        TCHAR path[MAX_DPATH];
        namep = curdir;
        ret = chdir (namep);
    }
#if 0
    write_log("curdir=\"%s\" oldcur=\"%s\" ret=%d\n", curdir, oldcur, ret);
#endif
    return ret;
#endif
}

bool my_resolvesoftlink(TCHAR *linkfile, int size)
{
    return false;
}

bool my_isfilehidden (const TCHAR *path)
{
    return 0;
}

void my_setfilehidden (const TCHAR *path, bool hidden)
{
}

TCHAR *target_expand_environment (const TCHAR *path, TCHAR *out, int maxlen)
{
    if (!path)
        return NULL;
    return out;
}
