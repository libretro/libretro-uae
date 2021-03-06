#pragma once
#ifndef SRC_INCLUDE_FILESYS_H_INCLUDED
#define SRC_INCLUDE_FILESYS_H_INCLUDED 1


 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Unix file system handler for AmigaDOS
  *
  * Copyright 1997 Bernd Schmidt
  */

struct hardfilehandle;
struct mountedinfo;
struct uaedev_config_info;
struct uae_prefs;

#define MAX_HDF_CACHE_BLOCKS 128
#define MAX_SCSI_SENSE 36
struct hdf_cache
{
	bool valid;
	uae_u8 *data;
	uae_u64 block;
	bool dirty;
	int readcount;
	int writecount;
	time_t lastaccess;
};

struct hardfiledata {
    uae_u64 virtsize; // virtual size
    uae_u64 physsize; // physical size (dynamic disk)
    uae_u64 offset;
    struct uaedev_config_info ci;
    struct hardfilehandle *handle;
    int handle_valid;
    int dangerous;
    int flags;
    uae_u8 *cache;
    int cache_valid;
    uae_u64 cache_offset;
    TCHAR vendor_id[8 + 1];
    TCHAR product_id[16 + 1];
    TCHAR product_rev[4 + 1];
    TCHAR device_name[256];
    /* geometry from possible RDSK block */
    int rdbcylinders;
    int rdbsectors;
    int rdbheads;
    uae_u8 *virtual_rdb;
    uae_u64 virtual_size;
    int unitnum;
    int byteswap;
    int adide;
    int hfd_type;

    uae_u8 *vhd_header;
    uae_u32 vhd_bamoffset;
    uae_u32 vhd_bamsize;
    uae_u32 vhd_blocksize;
    uae_u8 *vhd_sectormap;
    uae_u64 vhd_sectormapblock;
    uae_u32 vhd_bitmapsize;
    uae_u64 vhd_footerblock;

	void *chd_handle;

    int drive_empty;
    TCHAR *emptyname;

	struct hdf_cache bcache[MAX_HDF_CACHE_BLOCKS];
	uae_u8 scsi_sense[MAX_SCSI_SENSE];

	struct uaedev_config_info delayedci;
	int reinsertdelay;
	bool isreinsert;
};

#define HFD_FLAGS_REALDRIVE 1

struct hd_hardfiledata {
    struct hardfiledata hfd;
    uae_u64 size;
    int cyls;
    int heads;
    int secspertrack;
    int cyls_def;
    int secspertrack_def;
    int heads_def;
    int ansi_version;
};

#define HD_CONTROLLER_UAE 0
#define HD_CONTROLLER_IDE0 1
#define HD_CONTROLLER_IDE1 2
#define HD_CONTROLLER_IDE2 3
#define HD_CONTROLLER_IDE3 4
#define HD_CONTROLLER_SCSI0 5
#define HD_CONTROLLER_SCSI1 6
#define HD_CONTROLLER_SCSI2 7
#define HD_CONTROLLER_SCSI3 8
#define HD_CONTROLLER_SCSI4 9
#define HD_CONTROLLER_SCSI5 10
#define HD_CONTROLLER_SCSI6 11
#define HD_CONTROLLER_PCMCIA_SRAM 12
#define HD_CONTROLLER_PCMCIA_IDE 13

#define FILESYS_VIRTUAL 0
#define FILESYS_HARDFILE 1
#define FILESYS_HARDFILE_RDB 2
#define FILESYS_HARDDRIVE 3
#define FILESYS_CD 4

#define MAX_FILESYSTEM_UNITS 50

struct uaedev_mount_info;
extern struct uaedev_mount_info options_mountinfo;

struct hardfiledata *get_hardfile_data (int nr);
#define FILESYS_MAX_BLOCKSIZE 8192

int hdf_open (struct hardfiledata *hfd, const TCHAR *altname);
int hdf_dup (struct hardfiledata *dhfd, const struct hardfiledata *shfd);
void hdf_close (struct hardfiledata *hfd);
int hdf_read_rdb (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
int hdf_read (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
int hdf_write (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
int hdf_getnumharddrives (void);
TCHAR *hdf_getnameharddrive (int index, int flags, int *sectorsize, int *dangerousdrive);
int isspecialdrive(const TCHAR *name);
int get_native_path(uae_u32 lock, TCHAR *out);
void hardfile_do_disk_change (struct uaedev_config_data *uci, bool insert);
void hardfile_send_disk_change (struct hardfiledata *hfd, bool insert);
int hardfile_media_change (struct hardfiledata *hfd, struct uaedev_config_info *ci, bool inserted, bool timer);

void hdf_hd_close(struct hd_hardfiledata *hfd);
int hdf_hd_open(struct hd_hardfiledata *hfd);


int vhd_create (const TCHAR *name, uae_u64 size, uae_u32);

int hdf_init_target (void);
int hdf_open_target (struct hardfiledata *hfd, const TCHAR *name);
int hdf_dup_target (struct hardfiledata *dhfd, const struct hardfiledata *shfd);
void hdf_close_target (struct hardfiledata *hfd);
int hdf_read_target (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
int hdf_write_target (struct hardfiledata *hfd, void *buffer, uae_u64 offset, int len);
int hdf_resize_target (struct hardfiledata *hfd, uae_u64 newsize);
void getchsgeometry (uae_u64 size, int *pcyl, int *phead, int *psectorspertrack);
void getchsgeometry_hdf (struct hardfiledata *hfd, uae_u64 size, int *pcyl, int *phead, int *psectorspertrack);
void getchspgeometry (uae_u64 total, int *pcyl, int *phead, int *psectorspertrack, bool idegeometry);

void setsystime (void);

//int set_filesys_unit (int nr, struct uaedev_config_info *ci);
//int add_filesys_unit (struct uaedev_config_info *ci);

/** REMOVEME:
  * nowhere used
**/
#if 0
int hardfile_remount (int nr);
#endif // 0

#endif // SRC_INCLUDE_FILESYS_H_INCLUDED
