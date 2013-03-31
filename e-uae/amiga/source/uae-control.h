/*
 * UAE - The U*nix Amiga Emulator
 *
 * UAE-Control - Emulator control from inside emulation
 *  (c) 1996 Tauno Taipaleenmaki <tataipal@raita.oulu.fi>
 *  (c) 1998 Cloanto <info@cloanto.com>
 */

#define GAD_RESET                 1
#define GAD_DEBUG                 2
#define GAD_EXITEMU               3
#define GAD_EJECT_DF0             4
#define GAD_INSERT_DF0            5
#define GAD_EJECT_DF1             6
#define GAD_INSERT_DF1            7
#define GAD_EJECT_DF2             8
#define GAD_INSERT_DF2            9
#define GAD_EJECT_DF3             10
#define GAD_INSERT_DF3            11
#define GAD_SOUND                 12
#define GAD_JOYSTICK              13
#define GAD_FRAMERATE             14
#define GAD_CHIPMEMSIZE           15
#define GAD_SLOWMEMSIZE           16
#define GAD_FASTMEMSIZE           17
#define GAD_LANGUAGE              18

#define GAD_OK       1
#define GAD_CANCEL   2
#define GAD_STRING   3

/*
 * Configuration structure
 */
struct UAE_CONFIG
{
       ULONG             version;
       ULONG             chipmemsize;
       ULONG             slowmemsize;
       ULONG             fastmemsize;
       ULONG             framerate;
       ULONG             do_output_sound;
       ULONG             do_fake_joystick;
       ULONG             keyboard;
       UBYTE             disk_in_df0;
       UBYTE             disk_in_df1;
       UBYTE             disk_in_df2;
       UBYTE             disk_in_df3;
       char              df0_name[256];
       char              df1_name[256];
       char              df2_name[256];
       char              df3_name[256];
};
