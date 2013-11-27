#pragma once
#ifndef SRC_INCLUDE_AUTOCONF_H_INCLUDED
#define SRC_INCLUDE_AUTOCONF_H_INCLUDED 1

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Autoconfig device support
  *
  * (c) 1996 Ed Hanway
  */

#define RTAREA_DEFAULT 0xf00000
#define RTAREA_BACKUP  0xef0000
#define RTAREA_SIZE 0x10000
#define RTAREA_TRAPS 0x2000
#define RTAREA_RTG 0x3000
#define RTAREA_FSBOARD 0xFFEC
#define RTAREA_INT 0xFFEB

/* forward declarations */
#ifndef HAS_UAE_PREFS_STRUCT
struct uae_prefs;
#endif // HAS_UAE_PREFS_STRUCT

#ifndef HAS_UAEDEV_CONFIG_INFO
struct uaedev_config_info;
#endif // HAS_UAEDEV_CONFIG_INFO

#ifndef HAS_UAEDEV_CONFIG_DATA
struct uaedev_config_data;
#endif // HAS_UAEDEV_CONFIG_DATA

struct uaedev_mount_info;


/* external prototypes */
uae_u32 addr (int);
void db (uae_u8);
void dw (uae_u16);
void dl (uae_u32);
uae_u32 ds_ansi (const uae_char*);
uae_u32 ds (const char *);
uae_u32 ds_bstr_ansi (const uae_char *);
uae_u8 dbg (uaecptr);
void calltrap (uae_u32);
void org (uae_u32);
uae_u32 here (void);
uaecptr makedatatable (uaecptr resid, uaecptr resname, uae_u8 type, uae_s8 priority, uae_u16 ver, uae_u16 rev);

void align (int);

extern volatile int uae_int_requested;
void set_uae_int_flag (void);

#define RTS 0x4e75
#define RTE 0x4e73

extern uaecptr EXPANSION_explibname, EXPANSION_doslibname, EXPANSION_uaeversion;
extern uaecptr EXPANSION_explibbase, EXPANSION_uaedevname, EXPANSION_haveV36;
extern uaecptr EXPANSION_bootcode, EXPANSION_nullfunc;

extern uaecptr ROM_filesys_resname, ROM_filesys_resid;
extern uaecptr ROM_filesys_diagentry;
extern uaecptr ROM_hardfile_resname, ROM_hardfile_resid;
extern uaecptr ROM_hardfile_init;
extern uaecptr filesys_initcode;

int is_hardfile (int unit_no);
int nr_units (void);
int nr_directory_units (struct uae_prefs*);
uaecptr need_uae_boot_rom (void);

struct mountedinfo
{
	uae_u64 size;
	bool ismounted;
	bool ismedia;
	int nrcyls;
	TCHAR rootdir[MAX_DPATH];
};

int add_filesys_unitconfig (struct uae_prefs *p, int index, TCHAR *error);
int get_filesys_unitconfig (struct uae_prefs *p, int index, struct mountedinfo*);
int kill_filesys_unitconfig (struct uae_prefs *p, int nr);
int move_filesys_unitconfig (struct uae_prefs *p, int nr, int to);
char *validatedevicename (char *s);
char *validatevolumename (char *s);

int filesys_insert (int nr, const TCHAR *volume, const TCHAR *rootdir, bool readonly, int flags);
int filesys_eject (int nr);
int filesys_media_change (const TCHAR *rootdir, int inserted, struct uaedev_config_data *uci);

char *filesys_createvolname (const char *volname, const char *rootdir, const char *def);
int target_get_volume_name(struct uaedev_mount_info *mtinf, const char *volumepath, char *volumename, int size, int inserted, int fullcheck);

int sprintf_filesys_unit (char *buffer, int num);

void filesys_reset (void);
void filesys_cleanup (void);
void filesys_prepare_reset (void);
void filesys_start_threads (void);
void filesys_flush_cache (void);
void filesys_free_handles (void);
void filesys_vsync (void);

void filesys_install (void);
void filesys_install_code (void);
void filesys_store_devinfo (uae_u8 *);
void hardfile_install (void);
void hardfile_reset (void);
void emulib_install (void);
void expansion_init (void);
void expansion_cleanup (void);
void expansion_clear (void);

#endif // SRC_INCLUDE_AUTOCONF_H_INCLUDED