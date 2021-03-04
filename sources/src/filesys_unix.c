 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Miscellaneous support for virtual filesystems on Unix.
  *
  * Copyright 2004 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"

#include "filesys.h"

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#if HAVE_SYS_MOUNT_H
# include <sys/mount.h>
#endif

#if HAVE_SYS_VFS_H
# include <sys/vfs.h>
#endif

#if HAVE_SYS_STATFS_H
# include <sys/statfs.h>
#endif

#ifdef HAVE_SYS_STATVFS_H
# include <sys/statvfs.h>
#endif


/*
 * Try to determine whether the host filesystem
 * containing path is read-only - which, depending
 * on the host OS - isn't as easy as you might
 * think. ;-)
 *
 * Much more work to do here . . .
 */

int filesys_is_readonly (const char *path)
{
    /* If we don't know, we'll claim it's read/write */
    int readonly = 0;

#ifdef STAT_STATVFS

    /* With a working statvfs() it's easy */
    struct statvfs buf;

    if (statvfs (path, &buf) == 0)
      readonly = ((buf.f_flag & ST_RDONLY) == ST_RDONLY);
#else
# ifdef __linux__
# define ISOFS_SUPER_MAGIC 0x9660

    /* statvfs() is broken in many glibc implementations
     * so we don't use that. Instead, we'll be naive and
     * use statfs() and decide based on the filesystem
     * type. (We should be more clever and parse /etc/fstab
     * and /etc/mtab, but this'll do for just now).
     */
#ifdef __LIBRETRO__
# include <sys/statfs.h>
#endif

    struct statfs buf;

    if (statfs (path, &buf) == 0) {
		if (buf.f_type == ISOFS_SUPER_MAGIC)
		    readonly = 1;
    }

# endif
#endif
    return readonly;

}
