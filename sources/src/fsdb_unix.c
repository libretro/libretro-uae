 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Library of functions to make emulated filesystem as independent as
  * possible of the host filesystem's capabilities.
  * This is the Unix version.
  *
  * Copyright 1999 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "uae.h"
#include "fsdb.h"

/* these are deadly (but I think allowed on the Amiga): */
#define NUM_EVILCHARS 9
static TCHAR evilchars[NUM_EVILCHARS] = { '%', '\\', '*', '?', '\"', '/', '<', '>', '|' };
static char hex_chars[] = "0123456789abcdef";

#define UAEFSDB_BEGINS "__uae___"
#define UAEFSDB_BEGINSX "__uae___*"
#define UAEFSDB_LEN 604
#define UAEFSDB2_LEN 1632

/* The on-disk format is as follows:
* Offset 0, 1 byte, valid
* Offset 1, 4 bytes, mode
* Offset 5, 257 bytes, aname
* Offset 262, 257 bytes, nname
* Offset 519, 81 bytes, comment
* Offset 600, 4 bytes, Windows-side mode
*
* 1.6.0+ Unicode data
* 
* Offset  604, 257 * 2 bytes, aname
* Offset 1118, 257 * 2 bytes, nname
*        1632
*/

typedef struct fsdb_file_info {
    int type;
    uint32_t mode;
    int days;
    int mins;
    int ticks;
    char *comment;

} fsdb_file_info;

#define TRACING_ENABLED 0
#if TRACING_ENABLED
#define TRACE(x) do { write_log x; } while(0)
#else
#define TRACE(x)
#endif

int dos_errno (void)
{
    int e = errno;

    switch (e) {
	case ENOMEM:	return ERROR_NO_FREE_STORE;
	case EEXIST:	return ERROR_OBJECT_EXISTS;
	case EACCES:	return ERROR_WRITE_PROTECTED;
	case ENOENT:	return ERROR_OBJECT_NOT_AROUND;
	case ENOTDIR:	return ERROR_OBJECT_WRONG_TYPE;
	case ENOSPC:	return ERROR_DISK_IS_FULL;
	case EBUSY:     return ERROR_OBJECT_IN_USE;
	case EISDIR:	return ERROR_OBJECT_WRONG_TYPE;
#if defined(ETXTBSY)
	case ETXTBSY:	return ERROR_OBJECT_IN_USE;
#endif
#if defined(EROFS)
	case EROFS:     return ERROR_DISK_WRITE_PROTECTED;
#endif
#if defined(ENOTEMPTY)
#if ENOTEMPTY != EEXIST
	case ENOTEMPTY:	return ERROR_DIRECTORY_NOT_EMPTY;
#endif
#endif
	default:
	TRACE (("FSDB: Unimplemented error: %s\n", strerror (e)));
	return ERROR_NOT_IMPLEMENTED;
    }
}

#ifdef __LIBRETRO__
/* Return nonzero for any name we can't create on the native filesystem.  */
static int fsdb_name_invalid_2x (const TCHAR *n, int dir)
{
    int i;
    static char s1[MAX_DPATH];
    static TCHAR s2[MAX_DPATH];
    TCHAR a = n[0];
    TCHAR b = (a == '\0' ? a : n[1]);
    TCHAR c = (b == '\0' ? b : n[2]);
    TCHAR d = (b == '\0' ? c : n[3]);
    int l = _tcslen (n), ll;

    /* the reserved fsdb filename */
    if (_tcscmp (n, FSDB_FILE) == 0)
        return -1;

    if (dir) {
        if (n[0] == '.' && l == 1)
            return -1;
        if (n[0] == '.' && n[1] == '.' && l == 2)
            return -1;
    }

    if (a >= 'a' && a <= 'z')
        a -= 32;
    if (b >= 'a' && b <= 'z')
        b -= 32;
    if (c >= 'a' && c <= 'z')
        c -= 32;

    s1[0] = 0;
    s2[0] = 0;
    ua_fs_copy (s1, MAX_DPATH, n, -1);
    au_fs_copy (s2, MAX_DPATH, s1);
    if (_tcscmp (s2, n) != 0)
        return 1;

#ifdef WIN32
    /* reserved dos devices */
    ll = 0;
    if (a == 'A' && b == 'U' && c == 'X') ll = 3; /* AUX  */
    if (a == 'C' && b == 'O' && c == 'N') ll = 3; /* CON  */
    if (a == 'P' && b == 'R' && c == 'N') ll = 3; /* PRN  */
    if (a == 'N' && b == 'U' && c == 'L') ll = 3; /* NUL  */
    if (a == 'L' && b == 'P' && c == 'T'  && (d >= '0' && d <= '9')) ll = 4; /* LPT# */
    if (a == 'C' && b == 'O' && c == 'M'  && (d >= '0' && d <= '9')) ll = 4; /* COM# */
    /* AUX.anything, CON.anything etc.. are also illegal names */
    if (ll && (l == ll || (l > ll && n[ll] == '.')))
        return 3;

    /* spaces and periods at the end are a no-no */
    i = l - 1;
    if (n[i] == '.' || n[i] == ' ')
        return 1;
#endif

    /* these characters are *never* allowed */
    for (i = 0; i < NUM_EVILCHARS; i++) {
        if (_tcschr (n, evilchars[i]) != 0)
            return 2;
    }

#ifdef __LIBRETRO__
    /* Also illegal chars */
    for (i = 0; i < l; i++)
    {
        if (n[i] < 0)
            return 2;
    }
#endif

    return 0; /* the filename passed all checks, now it should be ok */
}

static int fsdb_name_invalid_2 (a_inode *aino, const TCHAR *n, int dir)
{
	int v = fsdb_name_invalid_2x(n, dir);
#if 1
	return v;
#else
	if (v <= 1 || !aino)
		return v;
	return 0;
#endif
}

int fsdb_name_invalid (a_inode *aino, const TCHAR *n)
{
    int v = fsdb_name_invalid_2 (aino, n, 0);
    if (v <= 0)
        return v;
    write_log (_T("FILESYS: '%s' illegal filename\n"), n);
    return v;
}

int fsdb_name_invalid_dir (a_inode *aino, const TCHAR *n)
{
    int v = fsdb_name_invalid_2 (aino, n, 1);
    if (v <= 0)
        return v;
    write_log (_T("FILESYS: '%s' illegal directory name\n"), n);
    return v;
}

static uae_u32 filesys_parse_mask(uae_u32 mask)
{
    return mask ^ 0xf;
}

int fsdb_exists (const char *nname)
{
    struct stat statbuf;
    return (stat (nname, &statbuf) != -1);
}
#else
/* Return nonzero for any name we can't create on the native filesystem.  */
int fsdb_name_invalid (const char *n)
{
    if (strcmp (n, FSDB_FILE) == 0)
	return 1;
    if (n[0] != '.')
	return 0;
    if (n[1] == '\0')
	return 1;
    return n[1] == '.' && n[2] == '\0';
}
#endif

/* For an a_inode we have newly created based on a filename we found on the
 * native fs, fill in information about this file/directory.  */
int fsdb_fill_file_attrs (a_inode *base, a_inode *aino)
{
	struct stat statbuf;
	/* This really shouldn't happen...  */
	if (stat (aino->nname, &statbuf) == -1)
		return 0;
	aino->dir = S_ISDIR (statbuf.st_mode) ? 1 : 0;
	aino->amigaos_mode = ((S_IXUSR & statbuf.st_mode ? 0 : A_FIBF_EXECUTE)
			  | (S_IWUSR & statbuf.st_mode ? 0 : A_FIBF_WRITE)
			  | (S_IRUSR & statbuf.st_mode ? 0 : A_FIBF_READ));
#ifdef __LIBRETRO__
	/* Always give execute & read permission */
	aino->amigaos_mode &= ~A_FIBF_EXECUTE;
	aino->amigaos_mode &= ~A_FIBF_READ;
	/* Force files under S as scripts */
	if ((strstr(aino->nname, "/S/") || strstr(aino->nname, "\\S\\"))
			&& !strstr(aino->nname, ".doc")
			&& !strstr(aino->nname, ".config")
			&& !strstr(aino->nname, ".prefs"))
		aino->amigaos_mode |= A_FIBF_SCRIPT;
#endif
    return 1;
}

int fsdb_set_file_attrs (a_inode *aino)
{
    struct stat statbuf;
    int mask = aino->amigaos_mode;
    int mode;

    if (stat (aino->nname, &statbuf) == -1)
	return ERROR_OBJECT_NOT_AROUND;

    mode = statbuf.st_mode;
    /* Unix dirs behave differently than AmigaOS ones.  */
    if (! aino->dir) {
	if (mask & A_FIBF_READ)
	    mode &= ~S_IRUSR;
	else
	    mode |= S_IRUSR;

	if (mask & A_FIBF_WRITE)
	    mode &= ~S_IWUSR;
	else
	    mode |= S_IWUSR;

	if (mask & A_FIBF_EXECUTE)
	    mode &= ~S_IXUSR;
	else
	    mode |= S_IXUSR;

	chmod (aino->nname, mode);
    }

    aino->amigaos_mode = mask;
    aino->dirty = 1;
    return 0;
}

/* Return nonzero if we can represent the amigaos_mode of AINO within the
 * native FS.  Return zero if that is not possible.  */
#if 1
int fsdb_mode_representable_p (const a_inode *aino, int amigaos_mode)
{
	int mask = amigaos_mode ^ 15;

	if (0 && aino->dir)
		return amigaos_mode == 0;

	if (aino->vfso)
		return 1;
	if (mask & A_FIBF_SCRIPT) /* script */
		return 0;
	if ((mask & 15) == 15) /* xxxxRWED == OK */
		return 1;
	if (!(mask & A_FIBF_EXECUTE)) /* not executable */
		return 0;
	if (!(mask & A_FIBF_READ)) /* not readable */
		return 0;
	if ((mask & 15) == (A_FIBF_READ | A_FIBF_EXECUTE)) /* ----RxEx == ReadOnly */
		return 1;
	return 0;
}
#else
int fsdb_mode_representable_p (const a_inode *aino)
{
    if (aino->dir)
	return aino->amigaos_mode == 0;
    return (aino->amigaos_mode & (A_FIBF_DELETE | A_FIBF_SCRIPT | A_FIBF_PURE)) == 0;
}
#endif


static char *aname_to_nname(const char *aname, int ascii)
{
    size_t len          = strlen(aname);
    size_t result_len   = (len * 3) + sizeof((UAEFSDB_BEGINS));
    unsigned int repl_1 = UINT_MAX;
    unsigned int repl_2 = UINT_MAX;

    TCHAR a = aname[0];
    TCHAR b = (a == '\0' ? a : aname[1]);
    TCHAR c = (b == '\0' ? b : aname[2]);
    TCHAR d = (c == '\0' ? c : aname[3]);

    if (a >= 'a' && a <= 'z') a -= 32;
    if (b >= 'a' && b <= 'z') b -= 32;
    if (c >= 'a' && c <= 'z') c -= 32;

    // reserved dos devices in Windows
    size_t ll = 0;
    if (a == 'A' && b == 'U' && c == 'X') ll = 3; // AUX
    if (a == 'C' && b == 'O' && c == 'N') ll = 3; // CON
    if (a == 'P' && b == 'R' && c == 'N') ll = 3; // PRN
    if (a == 'N' && b == 'U' && c == 'L') ll = 3; // NUL
    if (a == 'L' && b == 'P' && c == 'T' && (d >= '0' && d <= '9')) ll = 4; // LPT#
    if (a == 'C' && b == 'O' && c == 'M' && (d >= '0' && d <= '9')) ll = 4; // COM#
    // AUX.anything, CON.anything etc.. are also illegal names in Windows
    if (ll && (len == ll || (len > ll && aname[ll] == '.'))) {
        repl_1 = 2;
    }

    // spaces and periods at the end are a no-no in Windows
    int ei = len - 1;
    if (aname[ei] == '.' || aname[ei] == ' ') {
        repl_2 = ei;
    }

    // allocating for worst-case scenario here (max replacements)
    char *buf = (char*) malloc(result_len);
    char *p = buf;

    int repl, is_evil, j;
    unsigned char x;
    for (unsigned int i = 0; i < len; i++) {
        x = (unsigned char) aname[i];
        repl    = 0;
        is_evil = 0;
        if (i == repl_1) {
            repl = 1;
        }
        else if (i == repl_2) {
            repl = 2;
        }
        else if (x < 32) {
            // these are not allowed on Windows
            repl = 1;
        }
        else if (ascii && x > 127) {
            repl = 1;
        }
        for (j = 0; j < NUM_EVILCHARS; j++) {
            if (x == evilchars[j]) {
                repl    = 1;
                is_evil = 1;
                break;
            }
        }

#ifdef __LIBRETRO__
        /* Convert superscript 2 and 3 to normal chars */
        if (x == 178 || x == 179)
        {
            repl = 1;
            x = (x == 178) ? '2' : '3';
        }
#endif

        if (i == len - 1) {
            // last character, we can now check the file ending
            if (len >= 5 && strncasecmp(aname + len - 5, ".uaem", 5) == 0) {
                // we don't allow Amiga files ending with .uaem, so we replace
                // the last character
                repl = 1;
            }
        }
        if (repl) {
            *p++ = is_evil ? '_' : x;
            ll = 1;
        }
        else {
            *p++ = x;
        }
    }
    *p++ = '\0';

    if (ascii) {
        return buf;
    }

    char *result = (char*) malloc(result_len);
    if (ll > 0) {
        _tcscpy(result, UAEFSDB_BEGINS);
        _tcscat(result, buf);
    } else {
        _tcscpy(result, buf);
    }

    free(buf);

#if 0
    write_log("aname_to_nname %s => %s\n", aname, result);
#endif
    return result;
}

static unsigned char char_to_hex(unsigned char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + c - 'a';
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + c - 'A';
    }
    return 0;
}

static char *nname_to_aname(const char *nname, int noconvert)
{
    char *cresult;
    int len = strlen(nname);
    cresult = strdup(nname);
    if (!cresult) {
        write_log("[WARNING] nname_to_aname %s => Failed\n", nname);
        return NULL;
    }

    char *result = strdup(cresult);
    unsigned char *p = (unsigned char *) result;
    for (int i = 0; i < len; i++) {
        unsigned char c = cresult[i];
        if (c == '%' && i < len - 2) {
            *p++ = (char_to_hex(cresult[i + 1]) << 4) |
                    char_to_hex(cresult[i + 2]);
            i += 2;
        } else {
            *p++ = c;
        }
    }
    *p++ = '\0';
    free(cresult);

#ifdef __LIBRETRO__
    result = string_replace_substring(result, strlen(result), UAEFSDB_BEGINS, strlen(UAEFSDB_BEGINS), "", strlen(""));
#endif
#if 0
    write_log("nname_to_aname %s => %s\n", nname, result);
#endif
    return result;
}

#if 1
TCHAR *fsdb_create_unique_nname(a_inode *base, const TCHAR *suggestion)
{
    char *nname = aname_to_nname(suggestion, 0);
    TCHAR *p = build_nname(base->nname, nname);
    free(nname);
    return p;
}
#else
char *fsdb_create_unique_nname (a_inode *base, const char *suggestion)
{
    char tmp[256] = "__uae___";
    strncat (tmp, suggestion, 240);
    for (;;) {
	int i;
	char *p = build_nname (base->nname, tmp);
	if (access (p, R_OK) < 0 && errno == ENOENT) {
	    printf ("unique name: %s\n", p);
	    return p;
	}
	free (p);

	/* tmpnam isn't reentrant and I don't really want to hack configure
	 * right now to see whether tmpnam_r is available...  */
	for (i = 0; i < 8; i++) {
	    tmp[i] = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[uaerand () % 63];
	}
    }
}
#endif

/* return supported combination */
int fsdb_mode_supported (const a_inode *aino)
{
	int mask = aino->amigaos_mode;
	if (0 && aino->dir)
		return 0;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~(A_FIBF_SCRIPT | A_FIBF_READ | A_FIBF_EXECUTE);
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~A_FIBF_WRITE;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~A_FIBF_DELETE;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	return 0;
}



/* Return 1 if the nname is a special host-only name which must be translated
 * to aname using fsdb.
 */
a_inode *custom_fsdb_lookup_aino_nname(a_inode *base, const TCHAR *aname);
int custom_fsdb_used_as_nname(a_inode *base, const TCHAR *nname)
{
    if (custom_fsdb_lookup_aino_nname (base, nname))
        return 1;
    return 0;
}

extern int my_existstype(const char *name, int mode);

static int fsdb_get_file_info(const char *nname, fsdb_file_info *info)
{
    int error = 0;
    info->comment = NULL;
    info->type = my_existstype(nname, 2);
    info->mode = 15;
    return error;
}

a_inode *custom_fsdb_lookup_aino_aname(a_inode *base, const TCHAR *aname)
{
    const char *nname = aname_to_nname(aname, 0);
    const char *full_nname = build_nname(base->nname, nname);

    if (!fsdb_name_invalid(base, aname))
        return 0;

    fsdb_file_info info;
    fsdb_get_file_info(full_nname, &info);
    if (!info.type) {
        if (info.comment) {
            free(info.comment);
            info.comment = NULL;
        }
        return NULL;
    }

    a_inode *aino = xcalloc(a_inode, 1);
    aino->aname = nname_to_aname(nname, 0);
    aino->nname = strdup(full_nname);
#if 0
    if (info.comment) {
        aino->comment = nname_to_aname(info.comment, 1);
        free(info.comment);
    }
    else {
        aino->comment = NULL;
    }
#endif
    aino->amigaos_mode = filesys_parse_mask(info.mode);
    aino->dir = info.type == 2;
    aino->has_dbentry = 0;
    aino->dirty = 0;
    aino->db_offset = 0;
    return aino;
}

a_inode *custom_fsdb_lookup_aino_nname(a_inode *base, const TCHAR *nname)
{
    if (!strstr(nname, UAEFSDB_BEGINS))
        return 0;

    const char *full_nname = build_nname(base->nname, nname);
    fsdb_file_info info;
    fsdb_get_file_info(full_nname, &info);
    if (!info.type) {
        if (info.comment) {
            free(info.comment);
            info.comment = NULL;
        }
        return NULL;
    }

    a_inode *aino = xcalloc(a_inode, 1);
    aino->aname = nname_to_aname(nname, 0);
    aino->nname = build_nname(base->nname, nname);
#if 0
    if (info.comment) {
        aino->comment = nname_to_aname(info.comment, 1);
        free(info.comment);
    }
    else {
        aino->comment = NULL;
    }
#endif
    aino->amigaos_mode = filesys_parse_mask(info.mode);
    aino->dir = info.type == 2;
    aino->has_dbentry = 0;
    aino->dirty = 0;
    aino->db_offset = 0;
    return aino;
}
