#ifndef UAE_SYSDEPS_H
#define UAE_SYSDEPS_H

/*
  * UAE - The Un*x Amiga Emulator
  *
  * Try to include the right system headers and get other system-specific
  * stuff right & other collected kludges.
  *
  * If you think about modifying this, think twice. Some systems rely on
  * the exact order of the #include statements. That's also the reason
  * why everything gets included unconditionally regardless of whether
  * it's actually needed by the .c file.
  *
  * Copyright 1996, 1997 Bernd Schmidt
  */

#if defined(__cplusplus)
#include <cstddef>
#include <cstdbool>
#else
#include <stddef.h>
/* Note: stdbool.h has a __cplusplus section, but as it is stated in
 * GNU gcc stdbool.h:
 * "Supporting <stdbool.h> in C++ is a GCC extension."
 */
#if defined(HAVE_STDBOOL_H)
#    include <stdbool.h>
#  else
#    ifndef HAVE__BOOL
#      define _Bool signed char
#    endif
#    define bool _Bool
#    define false 0
#    define true 1
#    define __bool_true_false_are_defined 1
#  endif // HAVE_STDBOOL_H
#endif // __cplusplus

#define UAE_RAND_MAX RAND_MAX
#define ECS_DENISE

#ifdef JIT
#define NATMEM_OFFSET natmem_offset
#else
#undef NATMEM_OFFSET
#  undef JIT_DEBUG
#  undef USE_UDIS86
#endif

/* Add some useful macros for debugging. If JIT_DEBUG
 * is not defined, these macros do nothing.
 * Note: These are defined here, because some other parts
 * like inputrecord.c do make use of them.
 */
#if defined(JIT_DEBUG) && defined(__GNUC__)
# define JITLOG(fmt, ...) { \
	char trace_info[1024]; \
	snprintf(trace_info, 256, "[JIT] %s:%d - %s : %s\n", basename(__FILE__), __LINE__, __FUNCTION__, fmt); \
	write_log(trace_info, __VA_ARGS__); \
}
# if defined(USE_UDIS86)
#   include <udis86.h>
#   if defined(__x86_64__)
#	  define UD_MODE 64
#   else
#	  define UD_MODE 32
#   endif // __x86_64__
#   define UDISFN(udis_func, udis_end) { \
	int dSize = (int)((uaecptr)(udis_end) - (uaecptr)(udis_func)); \
	if (dSize > 0) { \
		uint8_t* p = (uint8_t*)(udis_func); \
		for ( ; dSize && (!p[dSize-1] || (0x90 == p[dSize-1])); --dSize) ; /* Find ending */ \
		JITLOG("Disassembling %s (size %u bytes) @ 0x%p:", #udis_func, dSize, p) \
		for (int i = 0; i < dSize; i += 0x10) { \
			write_log("%08x ", i); \
			for (int j = 0; j < 16; ++j) \
				write_log("%s%02x", 8==j ? "  " : " ", p[i + j]); \
			write_log("\n"); \
		} \
		ud_t ud_obj; \
		ud_init(&ud_obj); \
		ud_set_input_buffer(&ud_obj, p, dSize); \
		ud_set_mode(&ud_obj, UD_MODE); \
		ud_set_syntax(&ud_obj, UD_SYN_INTEL); \
		while (dSize > 0) { \
				dSize -= ud_disassemble(&ud_obj); \
				JITLOG("%s", ud_insn_asm(&ud_obj)); \
		} \
	} else \
		JITLOG("Can't dissassemble %s, start (0x%08lx) is larger than end (0x%08lx)", \
				#udis_func, (uaecptr)udis_func, (uaecptr)udis_end) \
}
# else
#   define UDISFN(...) {}
# endif // defined(USE_UDIS86)
#else
# define JITLOG(...) {}
# define UDISFN(...) {}
#endif

#if defined __AMIGA__ || defined __amiga__
#include <devices/timer.h>
#endif

#if defined(__cplusplus)
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <climits>
#else
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#endif // __cplusplus

#ifndef __STDC__
#  ifdef _MSC_VER
#    error "M$ is no longer supported. Use WinUAE instead, it's great!"
#  else
#error "Your compiler is not ANSI. Get a real one."
#endif
#endif

#if defined(__cplusplus)
#include <cstdarg>
#else
#include <stdarg.h>
#endif // __cplusplus

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_VALUES_H
#include <values.h>
#endif

#include "uae_string.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#ifndef stat64
#define stat64 stat
#endif
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
#  if defined(__cplusplus)
#    include <ctime>
#  else
# include <time.h>
#  endif // __cplusplus
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#    if defined(__cplusplus)
#      include <ctime>
#    else
#  include <time.h>
#    endif // __cplusplus
# endif
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
#else
# define dirent direct
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#ifdef HAVE_SYS_UTIME_H
# include <sys/utime.h>
#endif

#if EEXIST == ENOTEMPTY
#define BROKEN_OS_PROBABLY_AIX
#endif

#ifdef __NeXT__
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_ISDIR(val) (S_IFDIR & val)
struct utimbuf
{
    time_t actime;
    time_t modtime;
};
#endif

#include "uae_types.h"
#include "uae_malloc.h"
#include "writelog.h"

// Support macros for working with pointers of variable size -> 32bit.
#ifdef __x86_64__
#  if defined(JIT_DEBUG)
static uae_u32 jit_debug_to_uae_u32(uaecptr ptr, const char* macro, const char* file, const int line, const char* func)
{
	uae_u32 result = (uae_u32)ptr;
	if ((uaecptr)result != ptr) {
		write_log("%s:%d - %s : %s - Pointer Truncated! 0x%08x %08x\n",
				file, line, func, macro, (uae_u32)(ptr >> 32), result);
	}
	return result;
}
#    define PTR_TO_UINT32(ptr) jit_debug_to_uae_u32((uaecptr)ptr, "PTR_TO_UINT32", \
										basename(__FILE__), __LINE__, __FUNCTION__)
#  else
#    define PTR_TO_UINT32(ptr) ((uae_u32)(uaecptr)(ptr))
#  endif
#else
#  define PTR_TO_UINT32(ptr) ((uae_u32)(ptr))
#endif // __x86_64__
#define VALUE_TO_PTR(val)  ((uaecptr)(val))
#define PTR_OFFSET(src, dst) (((uaecptr)(dst)) - ((uaecptr)(src)))
#define NATMEM_ADDRESS     PTR_TO_UINT32(NATMEM_OFFSET)

/* The following macro allows to execute machine code directly
 * without the need of (illegal) object<->function pointer conversions.
 */
/// @todo : Add compiler/OS checks
#ifdef __GNUC__
#  define CALL_CODE_DIRECT(ptr) __asm__("call *%0" : : "a" (ptr));
#else
#  define CALL_CODE_DIRECT(ptr) ((compop_func*)ptr)(0);
#endif // __GNUC__


#ifdef __GNUC__
/* While we're here, make abort more useful.  */
#ifndef __MORPHOS__
/* This fails to compile on Morphos - not sure why yet */
#    define abort() { \
			write_log ("Internal error; file %s, line %d\n", __FILE__, __LINE__); \
			exit (0); \
		} // no need for a do-while!
#else
#define abort() exit(0)
#endif
#endif

/*
 * Porters to weird systems, look! This is the preferred way to get
 * filesys.c (and other stuff) running on your system. Define the
 * appropriate macros and implement wrappers in a machine-specific file.
 *
 * I guess the Mac port could use this (Ernesto?)
 */

#undef DONT_HAVE_POSIX
#undef DONT_HAVE_REAL_POSIX /* define if open+delete doesn't do what it should */
#undef DONT_HAVE_STDIO
#undef DONT_HAVE_MALLOC

#if defined(WARPUP)
#define DONT_HAVE_POSIX
#endif

#define FILEFLAG_DIR     0x1
#define FILEFLAG_ARCHIVE 0x2
#define FILEFLAG_WRITE   0x4
#define FILEFLAG_READ    0x8
#define FILEFLAG_EXECUTE 0x10
#define FILEFLAG_SCRIPT  0x20
#define FILEFLAG_PURE    0x40

#  if defined(__cplusplus)
extern "C" {
#  endif

#ifdef DONT_HAVE_POSIX
#define access posixemu_access
extern int posixemu_access (const char *, int);
#define open posixemu_open
extern int posixemu_open (const char *, int, int);
#define close posixemu_close
extern void posixemu_close (int);
#define read posixemu_read
extern int posixemu_read (int, char *, int);
#define write posixemu_write
extern int posixemu_write (int, const char *, int);
#undef lseek
#define lseek posixemu_seek
extern int posixemu_seek (int, int, int);
#define stat(a,b) posixemu_stat ((a), (b))
extern int posixemu_stat (const char *, STAT *);
#define mkdir posixemu_mkdir
extern int mkdir (const char *, int);
#define rmdir posixemu_rmdir
extern int posixemu_rmdir (const char *);
#define unlink posixemu_unlink
extern int posixemu_unlink (const char *);
#define truncate posixemu_truncate
extern int posixemu_truncate (const char *, long int);
#define rename posixemu_rename
extern int posixemu_rename (const char *, const char *);
#define chmod posixemu_chmod
extern int posixemu_chmod (const char *, int);
#define tmpnam posixemu_tmpnam
extern void posixemu_tmpnam (char *);
#define utime posixemu_utime
extern int posixemu_utime (const char *, struct utimbuf *);
#define opendir posixemu_opendir
extern DIR * posixemu_opendir (const char *);
#define readdir posixemu_readdir
extern struct dirent* readdir (DIR *);
#define closedir posixemu_closedir
extern void closedir (DIR *);

/* This isn't the best place for this, but it fits reasonably well. The logic
 * is that you probably don't have POSIX errnos if you don't have the above
 * functions. */
extern long dos_errno (void);
#endif

#ifdef DONT_HAVE_STDIO
extern FILE *stdioemu_fopen (const char *, const char *);
#define fopen(a,b) stdioemu_fopen(a, b)
extern int stdioemu_fseek (FILE *, int, int);
#define fseek(a,b,c) stdioemu_fseek(a, b, c)
extern int stdioemu_fread (char *, int, int, FILE *);
#define fread(a,b,c,d) stdioemu_fread(a, b, c, d)
extern int stdioemu_fwrite (const char *, int, int, FILE *);
#define fwrite(a,b,c,d) stdioemu_fwrite(a, b, c, d)
extern int stdioemu_ftell (FILE *);
#define ftell(a) stdioemu_ftell(a)
extern int stdioemu_fclose (FILE *);
#define fclose(a) stdioemu_fclose(a)
#endif

#ifdef DONT_HAVE_MALLOC
#define malloc(a) mallocemu_malloc(a)
extern void *mallocemu_malloc (int size);
#define free(a) mallocemu_free(a)
extern void mallocemu_free (void *ptr);
#endif

#ifdef X86_ASSEMBLY
#define ASM_SYM_FOR_FUNC(a) __asm__(a)
#else
#define ASM_SYM_FOR_FUNC(a)
#endif

#ifdef __CELLOS_LV2__
#define timezone 0
#define daylight 0
#endif

#ifdef ANDROID
#include "targets/t-android.h"
#else
#include "target.h"
#endif


//#include "target.h"
#if !defined(RECUR) && !defined(NO_MACHDEP)
#include "machdep/machdep.h"
#include "gfxdep/gfx.h"
#endif

extern void console_out (const char *, ...);
extern void console_flush (void);
extern int  console_get (char *, int);
extern void f_out (void *, const char *, ...);
extern void gui_message (const char *,...);
extern int gui_message_multibutton (int flags, const char *format,...);
#define write_log_err write_log

#if defined(__cplusplus)
}
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef STATIC_INLINE
#if __GNUC__ - 1 > 1 && __GNUC_MINOR__ - 1 >= 0
#define STATIC_INLINE static __inline__ __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
#define NORETURN __attribute__ ((noreturn))
#else
#define STATIC_INLINE static __inline__
#define NOINLINE
#define NORETURN
#endif
#endif

#ifndef MAX_PATH
#define MAX_PATH	512
#endif
#ifndef MAX_DPATH
#define MAX_DPATH	512
#endif

#ifndef __cplusplus
#define xmalloc(type, num) ((type*)malloc(sizeof (type) * (num)))
#define xcalloc(type, num) ((type*)calloc(sizeof (type), num))
#define xrealloc(type, buffer, num) ((type*)realloc(buffer, sizeof (type) * (num)))
#else
#define xmalloc(type, num) static_cast<type*>(malloc (sizeof (type) * (num)))
#define xcalloc(type, num) static_cast<type*>(calloc (sizeof (type), num))
#define xrealloc(type, buffer, num) static_cast<type*>(realloc (buffer, sizeof (type) * (num)))
#endif /* ! __cplusplus */

#define XFREE_DEBUG 0

// Please do NOT use identifiers with two leading underscores.
// See C++99 Standard chapter 7.1.3 "Reserved identifiers"
// #define __xfree(buffer) { free(buffer); buffer = NULL; }
#define xfree_d(buffer) { free(buffer); buffer = NULL; }

#if XFREE_DEBUG > 0
# define xfree(buffer) { \
	if (buffer) { \
		xfree_d (buffer) \
	} else { \
		fprintf (stderr, "NULL pointer freed at %s:%d %s\n", \
				__FILE__, __LINE__, __FUNCTION__); \
	} }
#else
# define xfree(buffer) { if (buffer) { xfree_d (buffer) } }
#endif //xfree_debug

#define DBLEQU(f, i) (abs ((f) - (i)) < 0.000001)

#ifndef _WIN32
#define TCHAR char
#endif
#define REGPARAM3
#define uae_char char
#define _tcslen strlen
#define _tcscpy strcpy
#define _tcscmp strcmp
#define _tcsncat strncat
#define _tcsncmp strncmp
#define _tstol atol
#define _totupper toupper
#define _stprintf sprintf
#define _tcscat strcat
#define _tcsicmp strcasecmp
#define _tcsnicmp strncasecmp
#define _tcsstr strstr
#define _tcsrchr strrchr
#define _tcsncpy strncpy
#define _tcschr strchr
#define _tstof atof
#define _istdigit isdigit
#define _istspace isspace
#define _tstoi atoi
#define _tcstol strtol
#define _wunlink unlink
#define _tcsftime strftime
#define vsntprintf vsnprint
#define _tcsspn strspn
#define _istupper isupper
#define _totlower tolower
#define _tcstok strtok
#define _wunlink unlink
#define _tfopen fopen
#define _vsntprintf vsnprintf
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#define _tcstod strtod
#define _T
#define sleep_millis uae_msleep

#define _istalnum iswalnum
#ifndef _WIN32
#define ULONG unsigned long
#endif
#define _strtoui64 strtoul
#define _tcscspn(wcs, reject) wcscspn((const wchar_t*)(wcs), (const wchar_t*)(reject))

#ifndef _stat64
#define _stat64 stat64
#endif /* _stat64 */

#ifndef offsetof
#  define offsetof(type, member)  __builtin_offsetof (type, member)
#endif /* offsetof */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
typedef int HANDLE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long long LONGLONG;
DWORD GetLastError(void);
#endif
#ifndef ANDROID
typedef int64_t off64_t;
#endif


#endif /* UAE_SYSDEPS_H */
