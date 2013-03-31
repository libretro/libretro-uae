 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Unix file system handler for AmigaDOS
  *
  * Copyright 1997 Bernd Schmidt
  */

struct hardfiledata {
    uae_u64 size;
    uae_u64 offset;
    int nrcyls;
    int secspertrack;
    int surfaces;
    int reservedblocks;
    unsigned int blocksize;
    void *handle;
    int readonly;
    int flags;
    uae_u8 *cache;
    int cache_valid;
    uae_u64 cache_offset;
    char vendor_id[8 + 1];
    char product_id[16 + 1];
    char product_rev[4 + 1];
    char device_name[256];
    /* geometry from possible RDSK block */
    unsigned int cylinders;
    unsigned int sectors;
    unsigned int heads;
    uae_u64 size2;
    uae_u64 offset2;
};

#define FILESYS_VIRTUAL 0
#define FILESYS_HARDFILE 1
#define FILESYS_HARDFILE_RDB 2
#define FILESYS_HARDDRIVE 3

#define FILESYS_FLAG_DONOTSAVE 1


//30
#define MAX_FILESYSTEM_UNITS 30

struct uaedev_mount_info;
extern struct uaedev_mount_info options_mountinfo;
extern void free_mountinfo (struct uaedev_mount_info *);

extern int nr_units (struct uaedev_mount_info *mountinfo);
extern int is_hardfile (struct uaedev_mount_info *mountinfo, int unit_no);
extern const char *set_filesys_unit (struct uaedev_mount_info *mountinfo, int,
				     const char *devname, const char *volname, const char *rootdir,
				     int readonly, int secs, int surfaces, int reserved,
				     int blocksize, int bootpri, const char *filesysdir, int flags);
extern const char *add_filesys_unit (struct uaedev_mount_info *mountinfo,
				     const char *devname, const char *volname, const char *rootdir,
				     int readonly, int secs, int surfaces, int reserved,
				     int blocksize, int bootpri, const char *filesysdir, int flags);
extern const char *get_filesys_unit (struct uaedev_mount_info *mountinfo, int nr,
				     char **devname, char **volame, char **rootdir, int *readonly,
				     int *secspertrack, int *surfaces, int *reserved,
				     int *cylinders, uae_u64 *size, int *blocksize, int *bootpri,
				     char **filesysdir, int *flags);
extern int kill_filesys_unit (struct uaedev_mount_info *mountinfo, int);
extern int move_filesys_unit (struct uaedev_mount_info *mountinfo, int nr, int to);
extern int sprintf_filesys_unit (const struct uaedev_mount_info *mountinfo, char *buffer, int num);
extern void write_filesys_config (const struct uaedev_mount_info *mountinfo, const char *unexpanded,
				  const char *defaultpath, FILE *f);


extern void filesys_init (void);
extern void filesys_reset (void);
extern void filesys_cleanup (void);
extern void filesys_prepare_reset (void);
extern void filesys_start_threads (void);
extern void filesys_flush_cache (void);

extern struct hardfiledata *get_hardfile_data (int nr);
#define FILESYS_MAX_BLOCKSIZE 2048
extern int hdf_open (struct hardfiledata *hfd, const char *name);
extern int hdf_dup (struct hardfiledata *dhfd, const struct hardfiledata *shfd);
extern void hdf_close (struct hardfiledata *hfd);
extern int hdf_read (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
extern int hdf_write (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
extern int hdf_getnumharddrives (void);
extern char *hdf_getnameharddrive (int index, int flags);
extern int hdf_init (void);
extern int isspecialdrive (const char *name);
extern void filesys_cleanup (void);
extern int filesys_is_readonly (const char *path);
