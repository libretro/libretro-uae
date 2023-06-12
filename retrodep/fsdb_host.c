#include "sysconfig.h"
#include "sysdeps.h"

#include "fsdb.h"
#include "zfile.h"

#include "libretro-core.h"

#ifdef USE_LIBRETRO_VFS
#undef local_to_utf8_string_alloc
#define local_to_utf8_string_alloc my_strdup
#endif

extern int log_filesys;

bool my_stat (const TCHAR *name, struct mystat *ms) {
	struct stat sonuc;
	if (stat(name, &sonuc) == -1) {
		if (log_filesys)
			write_log("my_stat: stat on file '%s' failed\n", name);
		return false;
	}
	if (log_filesys)
		write_log("stat returned size %9jd: %s\n", sonuc.st_size, name);
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

bool my_chmod (const TCHAR *name, uae_u32 mode)
{
    mode_t attr =   (S_IRUSR | S_IRGRP | S_IROTH);
    attr |=         (S_IXUSR | S_IXGRP | S_IXOTH);
    if (mode & FILEFLAG_WRITE)
        attr |=     (S_IWUSR | S_IWGRP | S_IWOTH);

        return chmod (name, attr);
}

static int setfiletime (const TCHAR *name, int days, int minute, int tick, int tolocal)
{
	return 0;
}

bool my_utime (const TCHAR *name, struct mytimeval *tv)
{
	int tolocal;
	int days, mins, ticks;
	struct mytimeval tv2;
	if (tv)
	{
		tv2.tv_sec = tv->tv_sec;
		tv2.tv_usec = tv->tv_usec;
		tolocal = 1;
	}
	else
	{
		tv2.tv_sec = 1;
		tv2.tv_usec = 1000;
		tolocal = 0;
	}
	timeval_to_amiga (&tv2, &days, &mins, &ticks, 50);
	if (setfiletime (name, days, mins, ticks, tolocal))
		return true;

	return false;
}

int my_existstype(const char *name, int mode)
{
	int ret = 0;

	const char *utf8 = local_to_utf8_string_alloc(name);

	if (path_is_valid(utf8))
	{
		switch (mode)
		{
			case 0: /* Dir */
				ret = path_is_directory(utf8) ? 1 : 0;
				break;
			case 1: /* File */
				ret = path_is_directory(utf8) ? 0 : 1;
				break;
			case 2: /* Dir/File */
				ret = path_is_directory(utf8) ? 2 : 1;
				break;
		}
	}

	return ret;
}

int my_existsfile(const char *name)
{
	return my_existstype(name, 1);
}

int my_existsdir(const char *name)
{
	return my_existstype(name, 0);
}

int my_getvolumeinfo(const char *root)
{
	struct stat sonuc;
	int ret = 0;

#ifdef USE_LIBRETRO_VFS
	if (stat(utf8_to_local_string_alloc(root), &sonuc) == -1)
#else
	if (stat(root, &sonuc) == -1)
#endif
		return -1;
	if (!S_ISDIR(sonuc.st_mode))
		return -1;

	ret |= MYVOLUMEINFO_STREAMS;
	return ret;
}

FILE *my_opentext(const TCHAR *name)
{
	FILE *f;
	uae_u8 tmp[4];
	int v;

	f = fopen(name, "rb");
	if (!f)
		return NULL;
	v = fread(tmp, 1, 4, f);
	fclose(f);
	if (v == 4) {
		if (tmp[0] == 0xef && tmp[1] == 0xbb && tmp[2] == 0xbf)
			return fopen (name, "r, ccs=UTF-8");
		if (tmp[0] == 0xff && tmp[1] == 0xfe)
			return fopen (name, "r, ccs=UTF-16LE");
	}
	return fopen(name, "r");
}

struct my_opendir_s *my_opendir(const TCHAR *name)
{
	struct my_opendir_s *mod;
	mod = xmalloc (struct my_opendir_s, 1);
	if (!mod)
		return NULL;

	RDIR* dir = retro_opendir(name);
	if (!dir) {
		write_log("my_opendir '%s' failed\n", name);
		xfree(mod);
		return NULL;
	}
	else if (log_filesys)
		write_log("my_opendir '%s'\n", name);

	mod->dh = dir;
	return mod;
}

void my_closedir(struct my_opendir_s *mod)
{
	if (mod)
	{
		retro_closedir(mod->dh);
		xfree(mod);
	}
}

int my_readdir(struct my_opendir_s* mod, TCHAR* name)
{
	mod->dp = retro_readdir(mod->dh);
	if (!mod->dp)
		return 0;
	_tcscpy (name, retro_dirent_get_name(mod->dh));
	if (log_filesys)
		write_log("my_readdir => '%s'\n", name);
	return 1;
}

int my_mkdir(const TCHAR *name)
{
#ifdef __WIN32__
	return mkdir(name);
#else
	return mkdir(name, 0777);
#endif
}

int my_rmdir(const TCHAR *name)
{
	struct my_opendir_s *od;
	int cnt;
	TCHAR tname[MAX_DPATH];
	memset(tname, 0, sizeof(TCHAR) * MAX_DPATH);

	/* SHFileOperation() ignores FOF_NORECURSION when deleting directories.. */
	od = my_opendir(name);
	if (!od)
		return -1;
	cnt = 0;
	while (my_readdir(od, tname)) {
		if (!_tcscmp(tname, _T(".")) || !_tcscmp(tname, _T("..")))
			continue;
		cnt++;
		break;
	}
	my_closedir(od);
	if (cnt > 0)
		return -1;

	return rmdir(name);
}

int my_unlink(const TCHAR *name, bool dontrecycle)
{
	return unlink(name);
}

int my_rename(const TCHAR *oldname, const TCHAR *newname)
{
	return rename(oldname, newname);
}

struct my_openfile_s *my_open(const TCHAR *name, int flags)
{
	const char *name_utf8 = local_to_utf8_string_alloc(name);

	if (log_filesys)
		write_log("my_open '%s' flags=%x\n", name, flags);

#ifdef FD_OPEN
	int open_flags = O_BINARY;
	if (flags & O_TRUNC)
		open_flags = open_flags | O_TRUNC;
	if (flags & O_CREAT)
		open_flags = open_flags | O_CREAT;

	if (flags & O_RDWR)
		open_flags = open_flags | O_RDWR;
	else if (flags & O_RDONLY)
		open_flags = open_flags | O_RDONLY;
	else if (flags & O_WRONLY)
		open_flags = open_flags | O_WRONLY;

	int fd = open(name, open_flags, 0644);
#else
	FILE *fp = NULL;
	char *fopen_flags = malloc(4*sizeof(char));
	strcpy(fopen_flags, "rb");

	if (flags & O_WRONLY)
		strcpy(fopen_flags, "wb");

	if (flags & O_RDWR)
		strcpy(fopen_flags, "rb+");

	if (flags & O_TRUNC)
		strcpy(fopen_flags, "wb+");

	fp = fopen(name_utf8, fopen_flags);

	free(fopen_flags);
	fopen_flags = NULL;
#endif

	struct my_openfile_s *mos;
	mos = xmalloc (struct my_openfile_s, 1);
	if (!mos)
		return NULL;

#ifdef FD_OPEN
	mos->fd = fd;
#else
	mos->fp = fp;
#endif
	mos->path = my_strdup(name);
	return mos;
}

void my_close(struct my_openfile_s* mos)
{
	if (log_filesys)
		write_log("my_close '%s'\n", mos->path);
#ifdef FD_OPEN
	int result = close(mos->fd);
#else
	int result = fclose(mos->fp);
#endif
	if (result != 0)
		write_log("error %d closing file '%s'\n", result, mos->path);
	xfree(mos->path);
	xfree(mos);
}

uae_s64 my_lseek(struct my_openfile_s *mos, uae_s64 offset, int whence)
{
	if (log_filesys)
		write_log("my_lseek '%s' %lld %d\n", mos->path, offset, whence);
#ifdef FD_OPEN
	off_t result = lseek(mos->fd, offset, whence);
#else
	off_t result = fseek(mos->fp, offset, whence);
	result = (result == 0) ? ftell(mos->fp) : -1;
#endif
	if (log_filesys)
		write_log("my_lseek result %jd\n", result);
	return result;
}

int my_truncate(const TCHAR *name, uae_u64 len) {
	int int_len = (int) len;
	if (log_filesys)
		write_log("my_truncate '%s' len = %d\n", name, int_len);
#ifdef FD_OPEN
	struct my_openfile_s *mos = my_open(name, O_WRONLY);
	if (mos == NULL) {
		write_log("WARNING: opening file for truncation failed\n");
		return -1;
	}
	int result = ftruncate(mos->fd, int_len);
	my_close(mos);
#else
	int result = truncate(name, int_len);
	if (result < 0)
		write_log("Truncate failed\n");
#endif
	return result;
}

uae_s64 my_fsize(struct my_openfile_s* mos) {
#ifdef FD_OPEN
	struct stat sonuc;
	if (fstat(mos->fd, &sonuc) == -1) {
		write_log("my_fsize: fstat on file '%s' failed\n", mos->path);
		return -1;
	}
	else if (log_filesys)
		write_log("my_fsize '%s' %d\n", mos->path, sonuc.st_size);

	return sonuc.st_size;
#else
	size_t size = 0;
	int current = ftell(mos->fp);
	if (fseek(mos->fp, 0, SEEK_END)) {
		write_log("my_fsize: fseek on file '%s' failed\n", mos->path);
		return -1;
	}

	size = ftell(mos->fp);
	if (log_filesys)
		write_log("my_fsize '%s' %d\n", mos->path, size);

	fseek(mos->fp, current, SEEK_SET);
	return size;
#endif
}

unsigned int my_read(struct my_openfile_s *mos, void *b, unsigned int size) {
#ifdef FD_OPEN
	ssize_t bytes_read = read(mos->fd, b, size);
#else
	ssize_t bytes_read = fread(b, 1, size, mos->fp);
#endif
	if (log_filesys)
		write_log("my_read size=%d => %zd\n", size, bytes_read);
	return (unsigned int) bytes_read;
}

unsigned int my_write(struct my_openfile_s *mos, void *b, unsigned int size) {
#ifdef FD_OPEN
	ssize_t bytes_written = write(mos->fd, b, size);
#else
	ssize_t bytes_written = fwrite(b, 1, size, mos->fp);
#endif
	if (log_filesys)
		write_log("my_write buffer=%p size=%d => %zd\n", b, size, bytes_written);
	return (unsigned int) bytes_written;
}

void my_canonicalize_path(const TCHAR *path, TCHAR *out, int size)
{
	_tcsncpy(out, path, size);
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

bool my_issamepath(const TCHAR *path1, const TCHAR *path2)
{
	TCHAR path1o[MAX_DPATH], path2o[MAX_DPATH];
	my_canonicalize_path(path1, path1o, sizeof path1o / sizeof(TCHAR));
	my_canonicalize_path(path2, path2o, sizeof path2o / sizeof(TCHAR));
	if (!_tcsicmp(path1o, path2o))
		return true;
	return false;
}

int my_setcurrentdir(const TCHAR *curdir, TCHAR *oldcur)
{
#ifdef VITA
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

bool my_createshortcut(const TCHAR *source, const TCHAR *target, const TCHAR *description) 
{
	return false;
}

bool my_resolvesoftlink(TCHAR *linkfile, int size, bool linkonly)
{
	return false;
}

const TCHAR *my_getfilepart(const TCHAR *filename)
{
	const TCHAR *p;

	p = _tcsrchr(filename, '\\');
	if (p)
		return p + 1;
	p = _tcsrchr(filename, '/');
	if (p)
		return p + 1;
	return filename;
}

bool my_isfilehidden(const TCHAR *path)
{
	return 0;
}

void my_setfilehidden(const TCHAR *path, bool hidden)
{
}

TCHAR *target_expand_environment(const TCHAR *path, TCHAR *out, int maxlen)
{
	if (!path)
		return NULL;
	if (out == NULL) {
		return my_strdup (path);
	} else {
		_tcscpy(out, path);
		return out;
	}
}
