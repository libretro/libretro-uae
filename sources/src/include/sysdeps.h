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
#ifndef UAE_SYSDEPS_H
#define UAE_SYSDEPS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "sysconfig.h"

#ifndef UAE
#define UAE
#endif

#ifdef __cplusplus
#include <string>
using namespace std;
#else
#include <string.h>
#include <ctype.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

#ifdef _WIN32
/* Parameters are passed in ECX, EDX for both x86 and x86-64 (RCX, RDX).
 * For x86-64, __fastcall is the default, so it isn't really required. */
#define JITCALL __fastcall
#elif defined(CPU_x86_64)
/* Parameters are passed in RDI, RSI by default (System V AMD64 ABI). */
#define JITCALL
#elif defined(HAVE_FUNC_ATTRIBUTE_REGPARM)
/* Parameters are passed in EAX, EDX on x86 with regparm(2). */
#define JITCALL __attribute__((regparm(2)))
/* This was originally regparm(3), but as far as I can see only two register
 * params are supported by the JIT code. It probably just worked anyway
 * if all functions used max two arguments. */
#elif !defined(JIT)
#define JITCALL
#endif
#define REGPARAM
#define REGPARAM2 JITCALL
#define REGPARAM3 JITCALL

#if CPU_64_BIT
#define addrdiff(a, b) ((int)((a) - (b)))
#else
#define addrdiff(a, b) ((a) - (b))
#endif

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

#include "uae/types.h"

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


/* If char has more then 8 bits, good night. */
typedef unsigned char uae_u8;
typedef signed char uae_s8;
typedef char uae_char;

typedef struct { uae_u8 RGB[3]; } RGB;

#if SIZEOF_SHORT == 2
typedef unsigned short uae_u16;
typedef short uae_s16;
#elif SIZEOF_INT == 2
typedef unsigned int uae_u16;
typedef int uae_s16;
#else
#error No 2 byte type, you lose.
#endif

#if SIZEOF_INT == 4
typedef unsigned int uae_u32;
typedef int uae_s32;
#elif SIZEOF_LONG == 4
typedef unsigned long uae_u32;
typedef long uae_s32;
#else
#error No 4 byte type, you lose.
#endif

typedef uae_u32 uaecptr;

#undef uae_s64
#undef uae_u64

#if SIZEOF_LONG_LONG == 8
#define uae_s64 long long
#define uae_u64 unsigned long long
#define VAL64(a) (a ## LL)
#define UVAL64(a) (a ## uLL)
#elif SIZEOF___INT64 == 8
#define uae_s64 __int64
#define uae_u64 unsigned __int64
#define VAL64(a) (a)
#define UVAL64(a) (a)
#elif SIZEOF_LONG == 8
#define uae_s64 long;
#define uae_u64 unsigned long;
#define VAL64(a) (a ## l)
#define UVAL64(a) (a ## ul)
#endif

#ifndef BOOL
typedef int BOOL;
#endif

typedef long uae_atomic;
uae_atomic atomic_and(volatile uae_atomic *p, uae_u32 v);
uae_atomic atomic_or(volatile uae_atomic *p, uae_u32 v);
uae_atomic atomic_inc(volatile uae_atomic *p);
uae_atomic atomic_dec(volatile uae_atomic *p);
uae_u32 atomic_bit_test_and_reset(volatile uae_atomic *p, uae_u32 v);

#ifdef HAVE_STRDUP
extern char *x_strdup(const char *str);
#define my_strdup x_strdup
#else
extern char *my_strdup (const char *s);
#endif
extern void my_trim (TCHAR*);
extern TCHAR *my_strdup_trim (const TCHAR*);
extern void to_lower (TCHAR *s, int len);
extern void to_upper (TCHAR *s, int len);
extern int uaestrlen(const char*);
extern int uaetcslen(const TCHAR*);

#define ENUMDECL typedef enum
#define ENUMNAME(name) name

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

#ifdef ANDROID
#include "targets/t-android.h"
#else
#include "target.h"
#endif

#if !defined(RECUR) && !defined(NO_MACHDEP)
#include "machdep/machdep.h"
#endif

#define error_log     write_log
#define uae_log       write_log
#define write_dlog    write_log
#define write_log_err write_log
#define console_out   write_log
#define console_out_f
#define f_out
#define setconsolemode

extern void console_out (const char *, ...);
/*extern void console_out_f (const TCHAR *, ...);*/
extern void console_flush (void);
extern int  console_get (char *, int);
extern void flush_log (void);
extern void gui_message (const char *,...);
extern int gui_message_multibutton (int flags, const char *format,...);
extern void logging_init (void);
extern FILE *log_open (const TCHAR *name, int append, int bootlog, TCHAR*);
extern void log_close (FILE *f);
extern TCHAR *write_log_get_ts(void);
extern bool is_console_open(void);
extern void activate_console (void);
extern void open_console(void);
extern void reopen_console(void);

extern bool use_long_double;

#if defined(__cplusplus)
}
#endif

#ifndef O_BINARY
#define O_BINARY 0x8000
#endif
#ifndef O_TRUNC
#define O_TRUNC 0x0200
#endif
#ifndef O_CREAT
#define O_CREAT 0x0100
#endif
#ifndef O_RDWR
#define O_RDWR 0x0002
#endif
#ifndef O_WRONLY
#define O_WRONLY 0x0001
#endif
#ifndef O_RDONLY
#define O_RDONLY 0x0000
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

/* Every Amiga hardware clock cycle takes this many "virtual" cycles.  This
   used to be hardcoded as 1, but using higher values allows us to time some
   stuff more precisely.
   512 is the official value from now on - it can't change, unless we want
   _another_ config option "finegrain2_m68k_speed".

   We define this value here rather than in events.h so that gencpu.c sees
   it.  */
#define CYCLE_UNIT 512

/* This one is used by cfgfile.c.  We could reduce the CYCLE_UNIT back to 1,
   I'm not 100% sure this code is bug free yet.  */
#define OFFICIAL_CYCLE_UNIT 512

/*
 * You can specify numbers from 0 to 5 here. It is possible that higher
 * numbers will make the CPU emulation slightly faster, but if the setting
 * is too high, you will run out of memory while compiling.
 * Best to leave this as it is.
 */
#define CPU_EMU_SIZE 0

#ifndef __cplusplus
#define xmalloc(type, num) ((type*)malloc(sizeof (type) * (num)))
#define xcalloc(type, num) ((type*)calloc(sizeof (type), num))
#define xrealloc(type, buffer, num) ((type*)realloc(buffer, sizeof (type) * (num)))
#define xfree(type) free(type)
#else
#define xmalloc(type, num) static_cast<type*>(malloc (sizeof (type) * (num)))
#define xcalloc(type, num) static_cast<type*>(calloc (sizeof (type), num))
#define xrealloc(type, buffer, num) static_cast<type*>(realloc (buffer, sizeof (type) * (num)))
#define xfree(type) free(type)
#endif /* ! __cplusplus */

#if 0
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
#endif

#define DBLEQU(f, i) (abs ((f) - (i)) < 0.000001)

#ifdef HAVE_VAR_ATTRIBUTE_UNUSED
#define NOWARN_UNUSED(x) __attribute__((unused)) x
#else
#define NOWARN_UNUSED(x) x
#endif

#ifndef _WIN32
#define TCHAR char
#endif
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
#ifndef _tcsicmp
#define _tcsicmp strcasecmp
#endif
#ifndef _tcsnicmp
#define _tcsnicmp strncasecmp
#endif
#ifndef _tcscspn
#define _tcscspn strcspn
#endif
#define _tcsstr strstr
#define _tcsrchr strrchr
#define _tcsncpy strncpy
#define _tcschr strchr
#define _tstof atof
#define _istdigit isdigit
#define _istspace isspace
#define _tstoi atoi
#define _tcstol strtol
#define _tcstoul strtoul
#define _wunlink unlink
#define _tcsftime strftime
#define _tcsspn strspn
#define _istupper isupper
#define _totlower tolower
#define _tcstok strtok
#define _wunlink unlink
#define _tfopen fopen
#ifndef vsntprintf
#define vsntprintf vsnprint
#endif
#ifndef _vsntprintf
#define _vsntprintf vsnprintf
#endif
#ifndef _stscanf
#define _stscanf sscanf
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#define _tcstod strtod
#ifndef _istalnum
#define _istalnum iswalnum
#endif
#ifndef _istalpha
#define _istalpha iswalpha
#endif
#ifndef _istxdigit
#define _istxdigit isxdigit
#endif

#ifndef _WIN32
#define ULONG unsigned long
#endif

#ifndef strnicmp
#define strnicmp _tcsnicmp
#endif

#ifndef stricmp
#define stricmp strcasecmp
#endif

#ifndef _strtoui64
#define _strtoui64 strtoul
#endif

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
#ifndef HANDLE
typedef int HANDLE;
#endif
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long long LONGLONG;
DWORD GetLastError(void);
#endif
#ifndef ANDROID
typedef int64_t off64_t;
#endif

#ifndef O_NDELAY
#define O_NDELAY 0
#endif

#ifndef MAXUINT
#define MAXUINT ((unsigned int)~ ((unsigned int)0))
#endif
#ifndef MAXINT
#define MAXINT ((int) (MAXUINT >> 1))
#endif
#ifndef MININT
#define MININT ((int)~MAXINT)
#endif

/* Types */
#define UINT16 uint16_t
#define UINT32 uint32_t
typedef uint32_t uint32;
typedef uint8_t uint8;

typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

typedef int64_t INT64;
typedef int32_t INT32;
typedef int16_t INT16;
typedef int8_t INT8;

#include "misc.h"

#ifdef VITA
#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>
#define mkdir(name, mode) sceIoMkdir(name, mode)
#define rmdir(name) sceIoRmdir(name)
#define chdir(name) 0
#define chmod(a, b)
#define timezone()
#endif

#ifdef __PS3__
#include <ctype.h>
#include "ps3_headers.h"
#endif

#ifdef __LIBRETRO__
#include <retro_endianness.h>
#include "string/stdstring.h"
#ifdef WIIU
#include <features/features_cpu.h>
#endif
#endif /* __LIBRETRO__ */

#ifndef tzset
#define tzset()
#endif
#ifndef _tzset
#define _tzset()
#endif
#ifndef timezone
#define timezone 0
#endif
#ifndef _timezone
#define _timezone 0
#endif
#ifndef daylight
#define daylight 0
#endif
#ifndef _daylight
#define _daylight 0
#endif

#if !defined(log2) && defined(logb)
#define log2 logb
#endif
#if !defined(log2l) && defined(logbl)
#define log2l logbl
#endif

#endif /* UAE_SYSDEPS_H */
