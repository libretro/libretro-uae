/*
 * PUAE - The Un*x Amiga Emulator
 *
 * Windows 2 Linux
 *
 * Copyright 2012-2013 Mustafa 'GnoStiC' Tufan
 */

#include "zfile.h"

#include <sys/time.h>

__BEGIN_DECLS

struct timeb {
    time_t          time;
    unsigned short  millitm;
    short           timezone;
    short           dstflag;
};

extern int  ftime(struct timeb*  timebuf);

typedef int BOOL;

#ifndef WIN32PORT

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
#ifndef WIN32PORT
#define FILE_FLAG_OVERLAPPED		0x40000000
#define FILE_FLAG_NO_BUFFERING		0x20000000
#define FILE_FLAG_RANDOM_ACCESS		0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN	0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE	0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS	0x02000000
#define FILE_FLAG_POSIX_SEMANTICS	0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT	0x00200000
#define FILE_FLAG_OPEN_NO_RECALL	0x00100000
#endif
#define FILE_FLAG_FIRST_PIPE_INSTANCE	0x00080000

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

#ifndef WIN32PORT
#define FILE_READ_DATA		0x0001
#define FILE_WRITE_DATA		0x0002
#define FILE_APPEND_DATA	0x0004
#endif
#ifndef WIN32PORT
#define GENERIC_READ		FILE_READ_DATA
#define GENERIC_WRITE		FILE_WRITE_DATA
#endif
#define FILE_SHARE_READ		0x00000001
#define FILE_SHARE_WRITE	0x00000002
#define FILE_SHARE_DELETE	0x00000004

#ifndef WIN32PORT
typedef struct {
	DWORD LowPart;
	int32_t HighPart;
	LONGLONG QuadPart;
} LARGE_INTEGER;
#endif

#ifdef WIN32PORT
 #define S_IRGRP 00040
 #define S_IWGRP 00020
 #define S_IROTH 00004
 #define S_IWOTH 00002
#endif

/* fsdb_mywin32 */
bool my_stat (const TCHAR *name, struct mystat *statbuf)
{
	struct _stat64 st;
	uae_s64 foo_size;

	if (stat (name, &st) != -1) {
		foo_size = st.st_size;
		statbuf->size = foo_size;

		if (st.st_mode & (S_IWGRP | S_IWOTH)) {
			statbuf->mode = FILEFLAG_READ | FILEFLAG_WRITE;
		} else {
			statbuf->mode = FILEFLAG_READ;
		}

//S_IFREG: regular file
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			statbuf->mode |= FILEFLAG_DIR;
		}

/*		statbuf->mode = st->st_mode;
		uae_u64 t = (*(uae_s64 *)&st->st_mtime-((uae_s64)(369*365+89)*(uae_s64)(24*60*60)*(uae_s64)10000000));
		statbuf->mtime.tv_sec = t / 10000000;
		statbuf->mtime.tv_usec = (t / 10) % 1000000;
		return true;*/
	}
	return false;
}

static int setfiletime (const TCHAR *name, int days, int minute, int tick, int tolocal)
{
//FIXME
	return 0;
}

#if defined(__CELLOS_LV2__) || defined(WIN32PORT)
#warning LSTAT STAT
#define lstat stat
#endif

bool my_utime (const TCHAR *name, struct mytimeval *tv)
{
        int tolocal;
        int days, mins, ticks;
        struct mytimeval tv2;
#ifndef __CELLOS_LV2__
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

#if defined(__CELLOS_LV2__) 
#include <unistd.h> //stat() is defined here
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#define F_OK 0
#endif

int my_existsfile (const char *name)
{
        struct stat sonuc;
        if (lstat (name, &sonuc) == -1) {
                return 0;
        } else {
                if (!S_ISDIR(sonuc.st_mode))
                        return 1;
        }
        return 0;
}

int my_existsdir (const char *name)
{
        struct stat sonuc;

        if (lstat (name, &sonuc) == -1) {
                return 0;
        } else {
                if (S_ISDIR(sonuc.st_mode))
                        return 1;
        }
        return 0;
}

int my_getvolumeinfo (const char *root)
{
        struct stat sonuc;
        int ret = 0;

        if (lstat (root, &sonuc) == -1)
                return -1;
        if (!S_ISDIR(sonuc.st_mode))
                return -1;
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

struct my_opendir_s *my_opendir (const TCHAR *name)
{
/*
	struct my_opendir_s *mod;
	TCHAR tmp[MAX_DPATH];

	tmp[0] = 0;
	if (currprefs.win32_filesystem_mangle_reserved_names == false)
		_tcscpy (tmp, PATHPREFIX);
	_tcscat (tmp, name);
	_tcscat (tmp, _T("\\"));
	_tcscat (tmp, mask);
	mod = xmalloc (struct my_opendir_s, 1);
	if (!mod)
		return NULL;
	mod->h = FindFirstFile(tmp, &mod->fd);
	if (mod->h == INVALID_HANDLE_VALUE) {
		xfree (mod);
		return NULL;
	}
	mod->first = 1;
	return mod;
*/
	/// FIXME: opendir returns struct DIR*, please translate instead of casting.
	return (struct my_opendir_s*)opendir(name);
}

void my_closedir (struct my_opendir_s *mod) {
	if (mod)
		/// FIXME: closedir needs struct DIR*, please translate instead of casting.
		closedir((struct DIR*)mod);
//	xfree (mod);
}

struct dirent* my_readdir (struct my_opendir_s *mod, TCHAR *name) {
/*
	if (mod->first) {
		_tcscpy (name, mod->fd.cFileName);
		mod->first = 0;
		return 1;
	}
	if (!FindNextFile (mod->h, &mod->fd))
		return 0;
	_tcscpy (name, mod->fd.cFileName);
*/
	///FIXME: readdir needs struct DIR*, please translate instead of casting.
	return readdir((struct DIR*)mod);
}

#if 0
static int recycle (const TCHAR *name)
{
/*        DWORD dirattr = GetFileAttributesSafe (name);
        bool isdir = dirattr != INVALID_FILE_ATTRIBUTES && (dirattr & FILE_ATTRIBUTE_DIRECTORY);
        const TCHAR *namep;
        TCHAR path[MAX_DPATH];

        if (currprefs.win32_filesystem_mangle_reserved_names == false) {
                _tcscpy (path, PATHPREFIX);
                _tcscat (path, name);
                namep = path;
        } else {
                namep = name;
        }

        if (currprefs.win32_norecyclebin || isdir || currprefs.win32_filesystem_mangle_reserved_names == false) {
                if (isdir)
                        return RemoveDirectory (namep) ? 0 : -1;
                else
                        return DeleteFile (namep) ? 0 : -1;
        } else {
                SHFILEOPSTRUCT fos;
                HANDLE h;

                h = CreateFile (namep, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (h != INVALID_HANDLE_VALUE) {
                        LARGE_INTEGER size;
                        if (GetFileSizeEx (h, &size)) {
                                if (size.QuadPart == 0) {
                                        CloseHandle (h);
                                        return DeleteFile (namep) ? 0 : -1;
                                }
                        }
                        CloseHandle (h);
                }

                // name must be terminated by \0\0
                TCHAR *p = xcalloc (TCHAR, _tcslen (namep) + 2);
                int v;

                _tcscpy (p, namep);
                memset (&fos, 0, sizeof (fos));
                fos.wFunc = FO_DELETE;
                fos.pFrom = p;
                fos.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NORECURSION | FOF_SILENT;
                v = SHFileOperation (&fos);
                xfree (p);
                switch (v)
                {
                case 0xb7: //DE_ERROR_MAX
                case 0x7c: //DE_INVALIDFILES
                case 0x402: // "unknown error"
                        v = ERROR_FILE_NOT_FOUND;
                        break;
                case 0x75: //DE_OPCANCELLED:
                case 0x10000: //ERRORONDEST:
                case 0x78: //DE_ACCESSDENIEDSRC:
                case 0x74: //DE_ROOTDIR:
                        v = ERROR_ACCESS_DENIED;
                        break;
                }
                SetLastError (v);
                return v ? -1 : 0;
        }*/
	return 0;
}
#endif

int my_rmdir (const TCHAR *name)
{
        struct my_opendir_s *od;
        int cnt;
        TCHAR tname[MAX_DPATH];
        memset(tname, 0, sizeof(TCHAR) * MAX_DPATH);

        /* SHFileOperation() ignores FOF_NORECURSION when deleting directories.. */
        od = my_opendir (name);
        if (!od) {
//                SetLastError (ERROR_FILE_NOT_FOUND);
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
//                SetLastError (ERROR_CURRENT_DIRECTORY);
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
	/*
        const TCHAR *onamep, *nnamep;
        TCHAR opath[MAX_DPATH], npath[MAX_DPATH];

        if (currprefs.win32_filesystem_mangle_reserved_names == false) {
                _tcscpy (opath, PATHPREFIX);
                _tcscat (opath, oldname);
                onamep = opath;
                _tcscpy (npath, PATHPREFIX);
                _tcscat (npath, newname);
                nnamep = npath;
        } else {
                onamep = oldname;
                nnamep = newname;
        }
        return MoveFile (onamep, nnamep) == 0 ? -1 : 0;
	*/
	return rename(oldname, newname);
}
#ifndef WIN32PORT
static bool CloseHandle(HANDLE hObject) {
	if (!hObject)
		return false;

	if (hObject == INVALID_HANDLE_VALUE || hObject == (HANDLE)-1)
		return true;

	return true;
}
#endif
void my_close (struct my_openfile_s *mos)
{
	close (mos->h);
	xfree (mos);
}
#ifndef WIN32PORT
static DWORD SetFilePointer(HANDLE hFile, int32_t lDistanceToMove, int32_t *lpDistanceToMoveHigh, DWORD dwMoveMethod) {
	if (!hFile)
		return 0;

	LONGLONG offset = lDistanceToMove;
	if (lpDistanceToMoveHigh) {
		LONGLONG helper = *lpDistanceToMoveHigh;
		helper <<= 32;
		offset &= 0xFFFFFFFF;   // Zero out the upper half (sign ext)
		offset |= helper;
	}

	int nMode = SEEK_SET;
	if (dwMoveMethod == FILE_CURRENT)
		nMode = SEEK_CUR;
	else if (dwMoveMethod == FILE_END)
		nMode = SEEK_END;

	off64_t currOff;
	currOff = lseek(hFile, offset, nMode);

	if (lpDistanceToMoveHigh) {
		*lpDistanceToMoveHigh = (int32_t)(currOff >> 32);
	}

	return (DWORD)currOff;
}
#endif
uae_s64 my_lseek (struct my_openfile_s *mos, uae_s64 offset, int whence) {
	off_t result = lseek(mos->h, offset, whence);
	return result;
    
	LARGE_INTEGER li, old;

	old.QuadPart = 0;
	old.LowPart = SetFilePointer (mos->h, 0, &old.HighPart, FILE_CURRENT);
	if (old.LowPart == INVALID_SET_FILE_POINTER && GetLastError () != NO_ERROR)
		return -1;
	if (offset == 0 && whence == SEEK_CUR)
		return old.QuadPart;
	li.QuadPart = offset;
	li.LowPart = SetFilePointer (mos->h, li.LowPart, &li.HighPart,
		whence == SEEK_SET ? FILE_BEGIN : (whence == SEEK_END ? FILE_END : FILE_CURRENT));
	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError () != NO_ERROR)
		return -1;
	return old.QuadPart;
}
#ifndef WIN32PORT
static HANDLE CreateFile(const TCHAR *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	int flags = 0, mode = S_IRUSR | S_IRGRP | S_IROTH;
	if (dwDesiredAccess & FILE_WRITE_DATA) {
		flags = O_RDWR;
		mode |= S_IWUSR;
	} else if ( (dwDesiredAccess & FILE_READ_DATA) == FILE_READ_DATA)
		flags = O_RDONLY;
	else {
		return INVALID_HANDLE_VALUE;
	}

	switch (dwCreationDisposition) {
		case OPEN_ALWAYS:
			flags |= O_CREAT;
			break;
		case TRUNCATE_EXISTING:
			flags |= O_TRUNC;
			mode  |= S_IWUSR;
			break;
		case CREATE_ALWAYS:
			flags |= O_CREAT|O_TRUNC;
			mode  |= S_IWUSR;
			break;
		case CREATE_NEW:
			flags |= O_CREAT|O_TRUNC|O_EXCL;
			mode  |= S_IWUSR;
			break;
		case OPEN_EXISTING:
			break;
	}

	int fd = 0;
	mode = S_IRUSR | S_IWUSR;
#if !defined(__CELLOS_LV2__) && !defined(WIN32PORT)
	if (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING)
		flags |= O_SYNC;
#endif
#if !defined(__CELLOS_LV2__) && !defined(WIN32PORT)
	flags |= O_NONBLOCK;
#endif
	fd = open(lpFileName, flags, mode);

	if (fd == -1 && errno == ENOENT) {
		write_log("FS: error %d opening file <%s>, flags:%x, mode:%x.\n", errno, lpFileName, flags, mode);
		return INVALID_HANDLE_VALUE;
	}/* else {
		write_log ("FS: '%s' open successful\n", lpFileName);
	}*/
#if !defined(__CELLOS_LV2__) && !defined(WIN32PORT)
	// turn of nonblocking reads/writes
	fcntl(fd, F_GETFL, &flags);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
#endif
	return fd;
}
#endif

struct my_openfile_s *my_open (const TCHAR *name, int flags) {
	errno = 0;

	struct my_openfile_s *mos;
	HANDLE h;
	DWORD DesiredAccess = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD CreationDisposition = OPEN_EXISTING;
	DWORD FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	DWORD attr;
	const TCHAR *namep;
	namep = name;

	mos = xmalloc (struct my_openfile_s, 1);
	if (!mos)
		return NULL;
//	attr = GetFileAttributesSafe (name);
	if (flags & O_TRUNC)
		CreationDisposition = CREATE_ALWAYS;
	else if (flags & O_CREAT)
		CreationDisposition = OPEN_ALWAYS;
	if (flags & O_WRONLY)
		DesiredAccess = GENERIC_WRITE;
	if (flags & O_RDONLY) {
		DesiredAccess = GENERIC_READ;
		CreationDisposition = OPEN_EXISTING;
	}
	if (flags & O_RDWR)
		DesiredAccess = GENERIC_READ | GENERIC_WRITE;
//	if (CreationDisposition == CREATE_ALWAYS && attr != INVALID_FILE_ATTRIBUTES && (attr & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)))
//		SetFileAttributesSafe (name, FILE_ATTRIBUTE_NORMAL);
	h = CreateFile (namep, DesiredAccess, ShareMode, 0, CreationDisposition, FlagsAndAttributes, 0);
	if (h == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (err == ERROR_ACCESS_DENIED && (DesiredAccess & GENERIC_WRITE)) {
			DesiredAccess &= ~GENERIC_WRITE;
			h = CreateFile (namep, DesiredAccess, ShareMode, 0, CreationDisposition, FlagsAndAttributes, 0);
			if (h == INVALID_HANDLE_VALUE)
				err = GetLastError();
		}
		if (h == INVALID_HANDLE_VALUE) {
			write_log (_T("FS: failed to open '%s' %x %x err=%d\n"), namep, DesiredAccess, CreationDisposition, err);
			xfree (mos);
			mos = NULL;
			goto err;
		}
	}
	mos->h = h;
err:
//	write_log (_T("open '%s' | flags: %d | FS: %x | ERR: %s\n"), namep, flags, mos ? mos->h : 0, strerror(errno));
/*	char buffer[65];
	int gotten;
	gotten = read(mos->h, buffer, 10);
	buffer[gotten] = '\0';
	write_log("*** %s ***\n",buffer);
	lseek(mos->h, 0, SEEK_SET); */
	return mos;
}
#ifndef WIN32PORT
static BOOL SetEndOfFile(HANDLE hFile) {
	if (hFile) {
		off64_t currOff = lseek(hFile, 0, SEEK_CUR);
		if (currOff >= 0)
			return (ftruncate(hFile, currOff) == 0);
	}
	return false;
}
#endif
int my_truncate (const TCHAR *name, uae_u64 len) {
	HANDLE hFile;
	int result = -1;
	const TCHAR *namep;
	
	namep = name;

	if ((hFile = CreateFile (namep, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) ) != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER li;
		li.QuadPart = len;
		li.LowPart = SetFilePointer (hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError () != NO_ERROR) {
			write_log (_T("FS: truncate seek failure for %s to pos %d\n"), namep, len);
		} else {
			if (SetEndOfFile (hFile) == true)
				result = 0;
		}
		CloseHandle (hFile);
	} else {
		write_log (_T("FS: truncate failed to open %s\n"), namep);
	}
	return result;
}

uae_s64 my_fsize (struct my_openfile_s *mos) {
	uae_s64 cur, filesize;

	cur = lseek (mos->h, 0, SEEK_CUR);
	filesize = lseek (mos->h, 0, SEEK_END);
	lseek (mos->h, cur, SEEK_SET);
//	write_log (_T("FS: filesize <%d>\n"), filesize);
	return filesize;
}

int my_read (struct my_openfile_s *mos, void *b, unsigned int size) {
//        DWORD read = 0;
//        ReadFile (mos->h, b, size, &read, NULL);
	ssize_t bytesRead = read(mos->h, b, size);
//	write_log (_T("read <%d> | FS: %x | size: %d | ERR: %s\n"), bytesRead, mos->h, size, strerror(errno));

	return bytesRead;
}

int my_write (struct my_openfile_s *mos, void *b, unsigned int size) {
//        DWORD written = 0;
//        WriteFile (mos->h, b, size, &written, NULL);
	ssize_t written = write (mos->h, b, size);
//	write_log (_T("wrote <%d> | FS: %x | ERR: %s\n"), written, mos->h, strerror(errno));

	return written;
}
