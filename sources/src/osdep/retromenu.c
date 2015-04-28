/**********************************************
               Main Menu options

**********************************************/

/* few bits and pieces taken from winuae: */
/*==========================================================================
*
*  Copyright (C) 1996 Brian King
*
*  File:       win32gui.c
*  Content:    Win32-specific gui features for UAE port.
*
***************************************************************************/

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "disk.h"
#include "sleep.h"
#include "autoconf.h"

#include "custom.h"
#include "inputdevice.h"
#include "memory.h"
#include "savestate.h"
#include "filesys.h"
#include "zfile.h"
#include "gensound.h"

#include "libretro-glue.h"
extern unsigned short int bmp[TEX_WIDTH * TEX_HEIGHT];

#define MPOS_CFG_SELECT 0
#define MPOS_FLOPPY1 1
#define MPOS_FLOPPY2 2
#define MPOS_FLOPPY3 3
#define MPOS_FLOPPY4 4
#define MPOS_KICK_ROM 5

#define MPOS_MENU_HDF 6
#define MPOS_MENU_SAVE 7
#define MPOS_MENU_CPU 8
#define MPOS_MENU_VIDEO 9
#define MPOS_MENU_SOUND 10
#define MPOS_MENU_MISC 11
#define MPOS_RESET 12

#define MPOS_STATE_SLOT 0
#define MPOS_STATE_LOAD 1
#define MPOS_STATE_SAVE 2

#define MPOS_HDF0 0
#define MPOS_HDF1 1
#define MPOS_HDF2 2
#define MPOS_HDF3 3
#define MPOS_HDF4 4
#define MPOS_HDF5 5
#define MPOS_HDF6 6

#define MPOS_CPU_TYPE 0
#define MPOS_CPU_SPEED 1
#define MPOS_CPU_EXACT 2
#define MPOS_CPU_COMPAT 3 
#define MPOS_CPU_CHIP_MEM 4
#define MPOS_CPU_BOGO_MEM 5
#define MPOS_CPU_FAST_MEM 6

#define MPOS_VIDEO_CHIP 0
#define MPOS_VIDEO_SIGNAL 1
#define MPOS_VIDEO_FSKIP 2
#define MPOS_VIDEO_VSYNC 3
#define MPOS_VIDEO_BLITS 4
#define MPOS_VIDEO_COLLISION 5
#define MPOS_VIDEO_BLIT_EXACT 6
#define MPOS_VIDEO_SCAN_LINE 7

#define MPOS_SOUND_TYPE 0
#define MPOS_SOUND_INTERPOL 1
#define MPOS_SOUND_CLICKS 2
#define MPOS_SOUND_CLICKS_VOL 3

#define MPOS_MISC_LEDS 0 
#define MPOS_MISC_JOY_PORTS 1
#define MPOS_MISC_MOUSE_SPEED 2
#define MPOS_MISC_FLOPPY_SPEED 3
#define MPOS_MISC_QUIT 4

#define MENU_MAIN 0
#define MENU_FILE 1
#define MENU_HDF 2
#define MENU_SAVE 3
#define MENU_CPU 4
#define MENU_VIDEO 5
#define MENU_SOUND 6
#define MENU_MISC 7

#define ACTION_CROSS 1
#define ACTION_SQUARE 2
#define ACTION_IDLE 10

#define THUMB_X 340
#define THUMB_Y 42

#define MAX_DISK 6

void retro_uae_reset(int y){}

#ifdef __PS3__
#define SAVE_PATH "/dev_hdd0/HOMEBREW/UAE/"
#else
	#ifdef AND
	#define SAVE_PATH "mnt/sdcard/euae/save/"
	#else
	#define SAVE_PATH "../save/"
	#endif

#endif

extern int PAS;
#define ps2_mouse_speed PAS

extern int vsync_enabled;

char* statline[] = {
	"1 select file    0 exit",			//0
	"1 insert    2 eject    0 exit",		//1
	"1 enter    0 exit",					//2
	"1 confirm    0 exit",					//3
	"1 select    0 back",				//4
	"3 4 select    0 back",			//5
	"1 confirm    0 back",				//6
	"1 mount    2 unmount    0 exit",		//7
};

char* opt_scanline_name[] = {
	"off ", "10% ", "20% ", "30% ", "40% ", "50% ", "60% ", "70% ", "80% ", "90% ", "100%"
};


char* statline_msg[128] = {0};

char* config_name = NULL; 
char* kickrom_name = NULL;
char* hdf_name[MAX_DISK];
char memsize[16 * 2];

int menu_pos = 0;
int max_menu_pos;
int ovl_left = 0;
#ifdef ENABLE_LOG_SCREEN
int ovl_top = 20;
#else
int ovl_top = 40;
#endif

char tmpPath[512];

int screenId = 0;
int stateSlot = 1; // save state slot
int * thumb =  NULL;
char thumbLoaded = 0;
int statusId = -1;

extern int opt_scanline;

/***************************************************************
function declaration
****************************************************************/
char * getBogoSize(int val);
char * getMemorySize(int val, int index);
void update_title_bar(void);
void  init_main_screen(void);
void paint_status_bar(void);
int update_menu(void);
static int draw_menu_item(int index, char* name, char* value, int statId);
static int draw_menu_item2(int index, char* name, char* value, int statId);
void update_cpu_menu(void);
char* get_hdf_name(int index);
/*static*/ int hardfile_testrdb (char *filename);
void update_hdf_menu(void);
void update_video_menu(void);
void update_sound_menu(void);
void update_misc_menu(void);
void update_save_menu(void);
void update_main_menu(void);
int do_action(int action);
int action_state_menu(int action);
/*static*/ void mount_hdf(int index, char* name);
/*static*/ void unmount_all_hdfs(void);
int action_hdf_menu(int action);
int action_misc_menu(int action);
int action_sound_menu(int action);
int action_video_menu(int action);
int action_cpu_menu(int action);
int action_main_menu(int action);


#define pixbuf bmp
#define draw_rect(a,b,c,d,e)	DrawFBoxBmp(pixbuf,a,b,c,d,RGB565(22,23,26) )
#define draw_text(a,b,c,d) 	Draw_text(pixbuf,a,b, menu_pos == index ? 0xff00:0xffff,0x8080,1,2 ,64,c)
#define draw_text2(a,b,c,d) 	Draw_text(pixbuf,a,b, 0xffff,0x8080,1,2 ,64,c)
#define RETRO_FSEL();	\
	if(filebrowse==0){\
		filebrowse=1;\
		return 0;\
	}\
	else {\
		filebrowse=0;\
		strcpy(name,browse_result);\
		write_log("selected=%s \n", name);\
	}

#define PAD_DOWN 1
#define PAD_SQUARE 3
#define PAD_CROSS 2
#define PAD_UP -1
#define PAD_RIGHT 10
#define PAD_LEFT -10
#define PAD_CIRCLE 0
#define PAD_START 0
#define RETRO_X 4

extern int pauseg;
int idle_counter = 0;
int sav_event=0;

#ifdef AND
#define DEFAULT_PATH "/mnt/sdcard/euae/"
#else
#ifdef __CELLOS_LV2__
#define DEFAULT_PATH "/dev_hdd0/HOMEBREW/UAE/"
#else
#define DEFAULT_PATH "/"
#endif
#endif

int filebrowse=0;
char browse_result[512]="\0";

int fsel_file_exists(char * path) {
	FILE* f;
	f = fopen(path, "rb");
	if (f == NULL) {
		return 0;
	}
	fclose(f);
	return 1;
}

int fsel_file_create(char * path) {
	char data = 0;
	FILE* f;
	
	f = fopen(path, "w+b");
	if (f == NULL) {
		return -1;
	}
	fwrite(&data,1,1,f); 
	fclose(f);
	return 0;
}
/***************************************************************
function implementation
****************************************************************/

char * getBogoSize(int val) {
	switch (val) {
	        case 0x1C0000 : return "1.75M\0";
		case  0x80000 : return "512k\0";
	        case 0x100000 : return "1M\0";
		default: return "0";
	}
}
char * getMemorySize(int val, int index) {
	if (val == 0) return "0";
	if (val == 0x80000) return "512k\0";
	int meg = val / (0x100000);
	int k = (val % 0x100000) / 10240;
	sprintf(memsize + index* 16, "%i.%iM\0", meg, k);
	return (char *) memsize + index * 16;
}

//paint cpu chipset and memory information
void update_title_bar(void) {
	char * chipset = "OCS";

	if (currprefs.chipset_mask & CSMASK_AGA)
		chipset = "AGA";
	else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) && (currprefs.chipset_mask & CSMASK_ECS_DENISE))
		chipset = "ECS";
	else if (currprefs.chipset_mask & CSMASK_ECS_AGNUS)
		chipset = "ECS_AGNUS";
	else if (currprefs.chipset_mask & CSMASK_ECS_DENISE)
		chipset = "ECS_DENISE";

//memory
	char tmp[256];

	sprintf(tmp, "%d %s mem ch=%s fa=%s bo=%s\0", 
		get_cpu_model(),
		chipset,
		getMemorySize(currprefs.chipmem_size,0),
		getMemorySize(currprefs.fastmem_size,1),
		getBogoSize(currprefs.bogomem_size)
	);
	draw_rect(ovl_left+24*8,ovl_top+26,408,8,1); //clean the previous text off the scrren
	draw_text2(ovl_left+24*8,ovl_top+26, (unsigned char*) tmp , 0);		

}

void  init_main_screen(void) {

	if (config_name == NULL) {
		config_name = (char *) malloc(512);
		if (config_name) {
//FIXME RETRO
			sprintf(config_name, "%s\0", get_current_config_name());
		}
	}
	if (kickrom_name == NULL) {
		kickrom_name = (char *) malloc(512);
	}
	if (hdf_name[0] == NULL) {
		int i;
		for (i = 0; i < MAX_DISK; i++) {
			hdf_name[i] = (char *) malloc(512);
		}
	}

	draw_rect(ovl_left+0,ovl_top+20,600,410,1);

	draw_text2(ovl_left+10,ovl_top+26, (unsigned char*)PACKAGE_STRING, 0);

	sprintf(kickrom_name, "%s\0", currprefs.romfile);

	update_title_bar();

	screenId = MENU_MAIN;
	max_menu_pos = 12;
	thumbLoaded = 0;
	update_menu();
}

void paint_status_bar(void) {
	
	if (statusId < 0) {
		return;
	}
	//draw either status line help text or the last message
	if (statline_msg[0] == 0) {
		draw_text2(ovl_left+10,ovl_top+406, (unsigned char*)statline[statusId], 0);
	} else {
		draw_text2(ovl_left+10,ovl_top+406, (unsigned char*)statline_msg, 0);
		statline_msg[0] = 0;
	}

}


void show_gui_message(char* msg) {
	if (msg == NULL) {
		return;
	}
	write_log ("gui message: %s \n", msg);

	int len = strlen(msg);
	if (len > 127) {
		len = 127;
	}
	if (msg[len - 1] == '\n') {
		len--;
	}
	if (len < 1) {
		return;
	}
	memset(statline_msg, 0, sizeof(statline_msg));
	memcpy(statline_msg, msg, len);
}

int update_menu(void) {
	int excludeArea = 0;
	//clean menu area 
	if (screenId != MENU_SAVE) {
		draw_rect(ovl_left+0,ovl_top+40,600,320,1);
	} else {
		excludeArea = (thumb != NULL && thumb[0]);
	}
	
	switch (screenId) {
		case MENU_MAIN:  update_main_menu(); break;
		case MENU_CPU:   update_cpu_menu(); break;
		case MENU_VIDEO: update_video_menu(); break;
		case MENU_SOUND: update_sound_menu(); break;
		case MENU_MISC:  update_misc_menu(); break;
		case MENU_SAVE:  update_save_menu(); break;
		case MENU_HDF:   update_hdf_menu(); break;
		
	} //end of switch
	
	paint_status_bar();

	return 0;
}

static int draw_menu_item(int index, char* name, char* value, int statId) {
	draw_text(ovl_left+10,ovl_top+50 + index * 24, (unsigned char*)name ,0);
	draw_text(ovl_left+118, ovl_top+50 + index * 24, (unsigned char*)value, 0);
	if (menu_pos == index) {
		statusId = statId;
	} 
	return 0;
}

//more space for the property name
static int draw_menu_item2(int index, char* name, char* value, int statId) {
	draw_text(ovl_left+10,ovl_top+50 + index * 24, (unsigned char*)name , 0);
	draw_text(ovl_left+178, ovl_top+50 + index * 24, (unsigned char*)value, 0);
	if (menu_pos == index) {
		statusId = statId;
	} 
	return 0;
}

void update_cpu_menu(void) {
	char string[32];
	
//RETRO FIXME
	char cpu_name[32];
	sprintf(cpu_name, "%d   \0", get_cpu_model());

	draw_menu_item2(MPOS_CPU_TYPE, " CPU type           ", cpu_name, 4 );
	
	if (currprefs.m68k_speed == 0) {
		strcpy(string, "real");
	} else 
	if (currprefs.m68k_speed == -1) {
		strcpy(string, "max ");
	} else {
		sprintf(string, "%d   \0", currprefs.m68k_speed/512);
	}
	draw_menu_item2(MPOS_CPU_SPEED,  " CPU speed          ", string, 4);
	
	draw_menu_item2(MPOS_CPU_EXACT,  " CPU cycle exact    ", currprefs.cpu_cycle_exact ? "on" : "off", 4);
	draw_menu_item2(MPOS_CPU_COMPAT, " CPU compatible     ", currprefs.cpu_compatible ? "on" : "off", 4);
	
	draw_menu_item2(MPOS_CPU_CHIP_MEM, " Chip memory        ", 	getMemorySize(currprefs.chipmem_size,0), 4);
	draw_menu_item2(MPOS_CPU_BOGO_MEM, " Bogo memory        ", 	getBogoSize(currprefs.bogomem_size), 4);
	draw_menu_item2(MPOS_CPU_FAST_MEM, " Fast memory        ", 	getMemorySize(currprefs.fastmem_size,0), 4);

	max_menu_pos = MPOS_CPU_FAST_MEM;
	update_title_bar();
	
}

char* get_hdf_name(int index) {

//FIXME RETRO

		int     secspertrack, surfaces, reserved, blocksize, bootpri;
		uae_u64 size;
		int     cylinders, readonly, flags;
		char   *devname, *volname, *rootdir, *filesysdir;
		const char *failure;

		/* We always use currprefs.mountinfo for the GUI.  The filesystem
		   code makes a private copy which is updated every reset.  */

			int  nosize = 0,type;
			struct uaedev_config_info *uci = &currprefs.mountconfig[index];
			struct mountedinfo mi;
			
			type = get_filesys_unitconfig (&currprefs, index, &mi);

			if (type < 0) {
				
				nosize = 1;
			}

		if(uci->type==FILESYS_HARDFILE)
			return mi.rootdir;
	
		return "\0";
}

/*static*/ int hardfile_testrdb (char *filename)
{
	struct zfile *f = zfile_fopen (filename, "rb",0);
	uae_u8 tmp[8];
	int i;
	int result = 0;

	if (!f)
		return -1;
	for (i = 0; i < 16; i++) {
		zfile_fseek (f, i * 512, SEEK_SET);
		memset (tmp, 0, sizeof tmp);
		zfile_fread (tmp, 1, sizeof tmp, f);
		/*
		if (i == 0 && !memcmp (tmp + 2, "CIS", 3)) {
			hdf->controller = HD_CONTROLLER_PCMCIA_SRAM;
			break;
		}
		*/
		if (
			!memcmp (tmp, "RDSK\0\0\0", 7) || 
			!memcmp (tmp, "DRKS\0\0", 6) || 
			(tmp[0] == 0x53 && tmp[1] == 0x10 && tmp[2] == 0x9b && tmp[3] == 0x13 && tmp[4] == 0 && tmp[5] == 0)) 
		{
			// RDSK or ADIDE "encoded" RDSK
			result = 1;
			break;
		}
	}
	zfile_fclose (f);
	return result;
}


void update_hdf_menu(void) {
	char string[64];
	char name[255];
	int i;
	int len;

//FIXME RETRO

	//draw hdf names
	for (i = 0; i < MAX_DISK; i++) {
		sprintf(name ,"%s\0",get_hdf_name(i));		
		if (name != NULL && name !="\0") {
			sprintf(hdf_name[i],"%s\0",name);

		} else {
			sprintf(name ,"\0");
			hdf_name[i][0] = 0;
		}
		sprintf(string	, " Disk %d      \0", i);
		draw_menu_item(MPOS_HDF0 + i, string, name, 7);
	}
	
	max_menu_pos = MPOS_HDF0 + MAX_DISK - 1;

	
}

static const TCHAR *sound_mode1[] = { _T("none"), _T("interrupts"), _T("normal"), _T("exact"), 0 };
static const TCHAR *soundmode2[] = { _T("none"), _T("interrupts"), _T("good"), _T("best"), 0 };
static const TCHAR *centermode1[] = { _T("none"), _T("simple"), _T("smart"), 0 };
static const TCHAR *centermode2[] = { _T("false"), _T("true"), _T("smart"), 0 };
static const TCHAR *stereomode[] = { _T("mono"), _T("stereo"), _T("clonedstereo"), _T("4ch"), _T("clonedstereo6ch"), _T("6ch"), _T("mixed"), 0 };
static const TCHAR *interpol_mode[] = { _T("none"), _T("anti"), _T("sinc"), _T("rh"), _T("crux"), 0 };
static const TCHAR *collmode[] = { _T("none"), _T("sprites"), _T("playfields"), _T("full"), 0 };


void update_video_menu(void) {
	char string[32];
	
	char * chipset = "OCS";

	if (currprefs.chipset_mask & CSMASK_AGA)
		chipset = "AGA";
	else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) && (currprefs.chipset_mask & CSMASK_ECS_DENISE))
		chipset = "ECS";
	else if (currprefs.chipset_mask & CSMASK_ECS_AGNUS)
		chipset = "ECS_AGNUS";
	else if (currprefs.chipset_mask & CSMASK_ECS_DENISE)
		chipset = "ECS_DENISE";

	draw_menu_item2(MPOS_VIDEO_CHIP,      " Chipset            ", chipset , 4);
	
	draw_menu_item2(MPOS_VIDEO_SIGNAL,    " Signal             ", currprefs.ntscmode ? "NTSC" : "PAL", 4);
	sprintf(string, "%d \0", currprefs.gfx_framerate);
	draw_menu_item2(MPOS_VIDEO_FSKIP,     " Frame skip         ", string, 4);
	draw_menu_item2(MPOS_VIDEO_VSYNC,     " Vsync              ", currprefs.gfx_apmode[0].gfx_vsync? "on ": "off", 4);
	draw_menu_item2(MPOS_VIDEO_BLITS,     " Immediate blits    ", currprefs.immediate_blits ? "on ": "off", 4);
	draw_menu_item2(MPOS_VIDEO_COLLISION, " Collision          ", (char*)collmode[currprefs.collision_level], 4);
	draw_menu_item2(MPOS_VIDEO_BLIT_EXACT," Blitter cyc. exact ", currprefs.blitter_cycle_exact ? "on": "off", 4);
	draw_menu_item2(MPOS_VIDEO_SCAN_LINE, " Scan lines         ", opt_scanline_name[opt_scanline], 4);

	max_menu_pos = MPOS_VIDEO_SCAN_LINE;
	
	update_title_bar();
	
}

void update_sound_menu(void) {
	char string[32];

	draw_menu_item2(MPOS_SOUND_TYPE,     " Sound              ", (char *)sound_mode1[currprefs.produce_sound], 4);
	draw_menu_item2(MPOS_SOUND_INTERPOL, " Interpolation      ", (char *)interpol_mode[currprefs.sound_interpol], 4);
	draw_menu_item2(MPOS_SOUND_CLICKS,   " Drive click        ", 0/*currprefs.dfxclick[0]*/ ? "on" : "off", 4);
	sprintf(string, "%d \0", (100/* - currprefs.dfxclickvolume*/));
	draw_menu_item2(MPOS_SOUND_CLICKS_VOL,  " Drive click volume ", string, 4);

	max_menu_pos = MPOS_SOUND_CLICKS_VOL;	
}

void update_misc_menu(void) {
	char string[32];

	draw_menu_item2(MPOS_MISC_LEDS,       " LEDs               ", currprefs.leds_on_screen? "on ": "off", 4);	
	
#if 1	
	int j0 = currprefs.jports[0].id/ JSEM_JOYS; 
	int j1 = currprefs.jports[1].id/ JSEM_JOYS;
	if (j0 == 2 && j1 == 1) {
		strcpy(string, "mouse & joy-1  ");
	} else
	if (j0 == 1 && j1 == 1) {
		strcpy(string, "joy-1 & joy-2  ");
	} else {
		strcpy(string, "mouse & mouse");
	}

	draw_menu_item2(MPOS_MISC_JOY_PORTS, " Joy ports          ", string, 4);
#endif
	sprintf(string, "%d \0", ps2_mouse_speed);
	draw_menu_item2(MPOS_MISC_MOUSE_SPEED, " Mouse speed        ", string, 4);

	sprintf(string, "%d %%\0", currprefs.floppy_speed);
	draw_menu_item2(MPOS_MISC_FLOPPY_SPEED, " Floppy speed       ", string, 4);
	
	draw_menu_item2(MPOS_MISC_QUIT, " >>  QUIT E-UAE  << ", " ", 6);
		
	max_menu_pos = MPOS_MISC_QUIT;	
}

void update_save_menu(void) {
	char string[32];
	int excludeArea = (thumb != NULL && thumb[0]);

	if (excludeArea) {
		draw_rect(ovl_left+0,ovl_top+50,340,150,1);
	} else {
		draw_rect(ovl_left+0,ovl_top+50,600,150,1);
	} 

	sprintf(string, "%d\0", stateSlot);
	draw_menu_item2(MPOS_STATE_SLOT,    " Slot               ",  string, 5);
	draw_menu_item2(MPOS_STATE_LOAD,    "     >> LOAD <<     ", " ", 6);
	draw_menu_item2(MPOS_STATE_SAVE,    "     >> SAVE <<     ", " ", 6);
	

	max_menu_pos = MPOS_STATE_SAVE;	
	
}

void update_main_menu(void) {
	char string[32];
	char* name;
	int len;
	int i;

	//draw config name
	len = strlen(config_name);
	name = config_name;
	if (len > 64) { //28
		name += (len-64); //28		
	}
	draw_menu_item(MPOS_CFG_SELECT, " Config file ", name, 0);


	//draw floppy names
	for (i = 0; i < 4; i++) {
		sprintf(string	, " Floppy %d    \0", i);
		len = strlen(currprefs.floppyslots[i].df);
		name = currprefs.floppyslots[i].df;
		if (len > 60) { //28
			name += (len-60); //28		
		}
		draw_menu_item(MPOS_FLOPPY1 + i, string, name, 1);
	}

	draw_menu_item(MPOS_KICK_ROM,   " Kick ROM    ", currprefs.romfile, 0);
	
	draw_menu_item(MPOS_MENU_HDF,   " Hard files  ", "=>", 2);
	draw_menu_item(MPOS_MENU_SAVE,  " Save state  ", "=>", 2);
	draw_menu_item(MPOS_MENU_CPU,   " CPU & RAM   ", "=>", 2);
	draw_menu_item(MPOS_MENU_VIDEO, " Video       ", "=>", 2);
	draw_menu_item(MPOS_MENU_SOUND, " Sound       ", "=>", 2);
	draw_menu_item(MPOS_MENU_MISC,  " Misc.       ", "=>", 2);

	draw_menu_item(MPOS_RESET,      " RESET       ", " ", 3);
	max_menu_pos = MPOS_RESET;

}

//action is either primary or secondary
int do_action(int action) {
	switch (screenId) {
		case MENU_MAIN:  return action_main_menu(action); break;
		case MENU_CPU:   return action_cpu_menu(action); break;
		case MENU_VIDEO: return action_video_menu(action); break;
		case MENU_SOUND: return action_sound_menu(action); break;
		case MENU_MISC:  return action_misc_menu(action); break;
		case MENU_SAVE:  return action_state_menu(action); break;
		case MENU_HDF:   return action_hdf_menu(action); break;
	} //end of switch
		
	return 0;
}


int action_state_menu(int action) {
	
	switch(menu_pos) {
		case MPOS_STATE_SLOT : {
			if (action == 3 || action == 1) { //right arrow or cross button
				stateSlot ++;
				if (stateSlot > 100) {
					stateSlot = 1;
				}
				thumbLoaded = 0;
				update_menu();

			} else
			if (action == 4) { //left arrow
				stateSlot --;
				if (stateSlot < 1) {
					stateSlot = 100;
				}
				thumbLoaded = 0;
				update_menu();
			} else
			//load thumbnail of the slot
			if (action == ACTION_IDLE && !thumbLoaded) {
				write_log("idle state -> load thumbnail\n");
				thumbLoaded = 1;
				sprintf(tmpPath, "%sstate%03d.img\0", SAVE_PATH, stateSlot);
				//thumb = ps3_load_thumbnail((unsigned char*) tmpPath, thumb);	
				update_menu();			
			}
		}; break;
		
		case MPOS_STATE_LOAD : {
			if (action == 1) {
				sprintf(tmpPath, "%sstate%03d.asf\0", SAVE_PATH, stateSlot);
				//check the file exists
				if (fsel_file_exists(tmpPath)) {
			//		savestate_initsave (tmpPath, 1);
					savestate_state = STATE_DORESTORE;
					return 1;
				}
				update_menu();
			} 

		} break;
		
		case MPOS_STATE_SAVE : {
			if (action == 1) {
				//save thumbnail
				sprintf(tmpPath, "%sstate%03d.img\0", SAVE_PATH, stateSlot);
				//ps3_save_thumb((unsigned char*)tmpPath);
			
				sprintf(tmpPath, "%sstate%03d.asf\0", SAVE_PATH, stateSlot);
				//bug ? if file doesn't exist it cannot be written to ??
				if (!fsel_file_exists(tmpPath)) {
					fsel_file_create(tmpPath);
				}
				
				//savestate_initsave (tmpPath, 1);
				save_state (tmpPath, "e-uae");
				return 1;
			} 
		} break;
		
	}
	return 0;
}

/*static*/ void mount_hdf(int index, char* name) {
//FIXME RETRO SET IF SLOT USED NOT ADD
	struct uaedev_config_info uci;

	int type = hardfile_testrdb (name);
	int secspertrack = 0;
	int surfaces = 0;
	int reserved = 0;
	int blocksize = 512;

	if (type < 0) {
		return;
	} else {
		//RDB file system -> use autodetection
		secspertrack = 0;
		surfaces = 0;
		reserved = 0;
		blocksize = 512;
					
		//Old File System -> use default values
		if (type == 0) {
			secspertrack = 32;
			surfaces = 1;
			reserved = 2;
		}
	}

	uci_set_defaults (&uci, false);
	uci.readonly = false;
	uci.type=UAEDEV_HDF;
	sprintf(uci.rootdir,"%s\0",name);
	add_filesys_config (&currprefs, -1, &uci);

}

/*static*/ void unmount_all_hdfs(void) {

	int units = nr_units(/*currprefs.mountinfo*/);
	while (units > 0) {
		units--;
		kill_filesys_unitconfig (&currprefs,units );
	}

}

int action_hdf_menu(int action) {

	char name[512];
	if (action == ACTION_IDLE) {
		return 0;
	}
	if (menu_pos >= MPOS_HDF0 && (menu_pos - MPOS_HDF0) < MAX_DISK) {
		int index = menu_pos - MPOS_HDF0;
		//mount disk
		if (action == ACTION_CROSS) {
			char* previousDiskName = NULL;
			if (index > 0 && hdf_name[index - 1] != NULL && hdf_name[index - 1][0] != 0) {
				previousDiskName = hdf_name[index - 1];
			}
		
			RETRO_FSEL();

			//write_log("hardfile selected=%s \n", name); 
			if (name != NULL) {
				mount_hdf(index, name);
			} 
		}
		else
		//unmount disk
		if (action == ACTION_SQUARE) {
			//check the hardfile name in the slot index exists
			if (hdf_name[index] != NULL && hdf_name[index][0] != 0) {
				kill_filesys_unitconfig (&currprefs, index);
			}
		}
	} // end if 	
	update_menu();

	return 0;
}

int action_misc_menu(int action) {
	if (action == ACTION_IDLE) {
		return 0;
	}
	switch(menu_pos) {
		case MPOS_MISC_LEDS : {
			if (currprefs.leds_on_screen) {
				currprefs.leds_on_screen = 0;
				clean_led_area();
		} else {
				currprefs.leds_on_screen = 1;
			}
		}; break;
		
		case MPOS_MISC_MOUSE_SPEED : {
			ps2_mouse_speed ++;
			if (ps2_mouse_speed > 6) {
				ps2_mouse_speed = 1;
			}
		}; break;
		
		case MPOS_MISC_JOY_PORTS : {

			int j0 = currprefs.jports[0].id/ JSEM_JOYS; 
			int j1 = currprefs.jports[1].id/ JSEM_JOYS;

			if (j0 == 2 && j1 == 1) { //mouse & joy-1
				//switch to joy-2 & joy-1
				changed_prefs.jports[0].id = JSEM_JOYS + 1;
				changed_prefs.mouse_settings[1]->enabled = 0;
			} else
			if (j0 == 1 && j1 == 1) { //joy-2 & joy-1
				//switch to mouse & mouse
				changed_prefs.jports[0].id = JSEM_MICE;
				changed_prefs.jports[1].id = JSEM_MICE + 1;
				changed_prefs.mouse_settings[1]->enabled = 1;
			} else { //mouse & mouse
				//switch to mouse & joy-1
				changed_prefs.jports[1].id = JSEM_JOYS;
				changed_prefs.mouse_settings[1]->enabled = 0;
			}
			inputdevice_config_change();
			check_prefs_changed_custom();

		}; break;
		
		case MPOS_MISC_FLOPPY_SPEED : {
			changed_prefs.floppy_speed += 100;
			if (changed_prefs.floppy_speed > 800) {
				changed_prefs.floppy_speed = 100;
			}else
			if (changed_prefs.floppy_speed > 400) {
				changed_prefs.floppy_speed = 800;
			}
			DISK_check_change();
		} break;
		
		case MPOS_MISC_QUIT : {
			/*
			if (uae_get_state() == UAE_STATE_STOPPED) {
			}
			write_log("uae state=%i \n", uae_get_state() );
			*/
			uae_quit();	
			retro_deinit();
			exit(0);
			return -1;
		} 
	} //end of switch
	update_menu();
	return 0;
}	

int action_sound_menu(int action) {
	if (action == ACTION_IDLE) {
		return 0;
	}
	switch(menu_pos) {
		case MPOS_SOUND_TYPE : {
			char* model =(char*)sound_mode1[currprefs.produce_sound + 1];
			if (model == NULL) {
				changed_prefs.produce_sound = 0;
			} else {
				changed_prefs.produce_sound = currprefs.produce_sound + 1;
			}
		}; break;
		
		case  MPOS_SOUND_INTERPOL : {
			char* mode =(char*)interpol_mode[currprefs.sound_interpol + 1];
			if (mode == NULL) {
				changed_prefs.sound_interpol = 0;
			} else {
				changed_prefs.sound_interpol = currprefs.sound_interpol + 1;
			}
		} break;
		
		case MPOS_SOUND_CLICKS : {
			//turn on / off clicks in all 4 floopy drives
/*
			changed_prefs.dfxclick[0] = 1 - currprefs.dfxclick[0];
			changed_prefs.dfxclick[1] = changed_prefs.dfxclick[0];
			changed_prefs.dfxclick[2] = changed_prefs.dfxclick[0];
			changed_prefs.dfxclick[3] = changed_prefs.dfxclick[0]; 
*/
		} break;
		
		case MPOS_SOUND_CLICKS_VOL : {
/*
			changed_prefs.dfxclickvolume = ((currprefs.dfxclickvolume / 10) * 10) - 10;
			if (changed_prefs.dfxclickvolume < 0) {
				changed_prefs.dfxclickvolume = 90;
			}
*/
		} break;
	} //end of switch
	config_changed = 1;
	check_prefs_changed_audio();
	update_menu();
	return 0;
}

int action_video_menu(int action) {
	if (action == ACTION_IDLE) {
		return 0;
	}
	switch(menu_pos) {
		case MPOS_VIDEO_CHIP: {

		//from AGA switch to OCS
		if (currprefs.chipset_mask & CSMASK_AGA) {
			changed_prefs.chipset_mask = currprefs.chipset_mask;
			changed_prefs.chipset_mask &= ~CSMASK_AGA; //remove AGA flag
			changed_prefs.chipset_mask &= ~(CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE);  //remove ECS flags
		}
		//from full ECS switch to AGA
		else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) && (currprefs.chipset_mask & CSMASK_ECS_DENISE)) {
			changed_prefs.chipset_mask = currprefs.chipset_mask;
			changed_prefs.chipset_mask |= CSMASK_AGA;  //keep ECS flags coz AGA chip supports ECS modes
		}
		//from OCS switch to ECS_AGNUS
		else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) == 0 && (currprefs.chipset_mask & CSMASK_ECS_DENISE) == 0 ) {
			changed_prefs.chipset_mask = currprefs.chipset_mask;
			changed_prefs.chipset_mask |= CSMASK_ECS_AGNUS;  
		}
		//from ECS_AGNUS switch to ECS_DENISE
		else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) == 1 && (currprefs.chipset_mask & CSMASK_ECS_DENISE) == 0 ) {
			changed_prefs.chipset_mask = currprefs.chipset_mask;
			changed_prefs.chipset_mask &= ~CSMASK_ECS_AGNUS;  
			changed_prefs.chipset_mask |= CSMASK_ECS_DENISE;  
		}
		//from ECS_DENISE switch to full ECS
		else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) == 0 && (currprefs.chipset_mask & CSMASK_ECS_DENISE) ) {
			changed_prefs.chipset_mask = currprefs.chipset_mask;
			changed_prefs.chipset_mask |= CSMASK_ECS_AGNUS;  
		}
		
		} break;
		
		case MPOS_VIDEO_SIGNAL: {
			changed_prefs.ntscmode = 1 - currprefs.ntscmode;
		} break;
		
		case MPOS_VIDEO_FSKIP : {
			changed_prefs.gfx_framerate++;
			if (changed_prefs.gfx_framerate > 20) {
				changed_prefs.gfx_framerate = 1;
			}
		} break;
		
		case MPOS_VIDEO_VSYNC : {
			changed_prefs.gfx_apmode[0].gfx_vsync = 1 - changed_prefs.gfx_apmode[0].gfx_vsync;
			vsync_enabled = changed_prefs.gfx_apmode[0].gfx_vsync;
		} break;
		
		case MPOS_VIDEO_BLITS : {
			changed_prefs.immediate_blits = 1 - changed_prefs.immediate_blits;
		} break;
		
		case MPOS_VIDEO_COLLISION : {
			char * mode = (char*)collmode[currprefs.collision_level + 1];
			if (mode == NULL) {
				changed_prefs.collision_level = 0;
			}  else {
				changed_prefs.collision_level = currprefs.collision_level+1;
			}
		} break;
		
		case MPOS_VIDEO_BLIT_EXACT: {
			changed_prefs.blitter_cycle_exact = 1 - currprefs.blitter_cycle_exact;
			currprefs.blitter_cycle_exact = changed_prefs.blitter_cycle_exact; 
		} break;

		case MPOS_VIDEO_SCAN_LINE: {
			if (action == 4) {
				opt_scanline--;
				if (opt_scanline < 0) {
					opt_scanline = 10;
				}
			} else {
				opt_scanline++;
				if (opt_scanline > 10) {
					opt_scanline = 0;
				}
			}
		//	draw_scanlines(1, gfxvidinfo.height, 1);
		} break;

		
	} // end of switch
	config_changed = 1;
	check_prefs_changed_custom();

	update_menu();
	return 0;
}

int MCPU[]= {68000, 68010,68020, 68030, 68040, 68060,-1};

int action_cpu_menu(int action) {
	if (action == ACTION_IDLE) {
		return 0;
	}
	switch(menu_pos) {
		case MPOS_CPU_TYPE: {
			
			int type=(currprefs.cpu_model-68000)/10;
			if(type==6)type=5;
			int index = MCPU[++type];
			if (index == -1) {
				index = MCPU[0];
				type = 0;
			}
			changed_prefs.cpu_model=index;

			if (changed_prefs.cpu_model <= 68020)changed_prefs.address_space_24=1;
			
		}; break;
		
		case MPOS_CPU_SPEED: {
			if (changed_prefs.m68k_speed > 0) {
				changed_prefs.m68k_speed /= 512;
			}
			changed_prefs.m68k_speed ++;
			if (changed_prefs.m68k_speed > 12) {
				changed_prefs.m68k_speed = -1;
			} else 

			if (changed_prefs.m68k_speed > 0) {
				changed_prefs.m68k_speed*=512;
			}
		};	break;
		
		case MPOS_CPU_EXACT: {
			changed_prefs.cpu_cycle_exact = 1 - currprefs.cpu_cycle_exact;
			//exact & compatible cannot be turned on both at the same time
			if (changed_prefs.cpu_cycle_exact && currprefs.cpu_compatible) {
				changed_prefs.cpu_compatible = 0;
			}
		}; break;
		
		case MPOS_CPU_COMPAT: {
			changed_prefs.cpu_compatible = 1 - currprefs.cpu_compatible;
			//exact & compatible cannot be turned on both at the same time
			if (changed_prefs.cpu_compatible && currprefs.cpu_cycle_exact) {
				changed_prefs.cpu_cycle_exact = 0;
			}
		}; break;	
		
		case MPOS_CPU_CHIP_MEM : {
			changed_prefs.chipmem_size = currprefs.chipmem_size;
			if (changed_prefs.chipmem_size == 0x80000) { // 512 kb
				changed_prefs.chipmem_size = 0x100000;
			} else
			{
				changed_prefs.chipmem_size += 0x100000;
				//if fastmem is present then max chip size is 2 Megs
				if (currprefs.fastmem_size != 0 && changed_prefs.chipmem_size > 0x200000) { 
					changed_prefs.chipmem_size = 0x80000; // back to 512 k
				} else
				//more than 8 megs of chip
				if (changed_prefs.chipmem_size > 0x800000) {
					changed_prefs.chipmem_size = 0x80000; // back to 512 k
				}
			}
			memory_reset();
		} ; break;	
		
		case MPOS_CPU_BOGO_MEM : {
			changed_prefs.bogomem_size = currprefs.bogomem_size;
			if (changed_prefs.bogomem_size == 0x80000) {
				changed_prefs.bogomem_size = 0x100000;
			} else
			if (changed_prefs.bogomem_size == 0x100000) {
				changed_prefs.bogomem_size = 0x1C0000;
			} else
			if (changed_prefs.bogomem_size == 0) {
				changed_prefs.bogomem_size = 0x80000;
			}
			else {
				changed_prefs.bogomem_size = 0;
			}
			memory_reset();
		} break;
		
		case MPOS_CPU_FAST_MEM : {
			changed_prefs.fastmem_size = (currprefs.fastmem_size / 0x100000) * 0x100000;
			changed_prefs.fastmem_size += 0x100000;
			if (changed_prefs.fastmem_size > 0x800000) {
				changed_prefs.fastmem_size = 0;
			}
			if (changed_prefs.fastmem_size > 0 && currprefs.chipmem_size > 0x200000) {
				changed_prefs.chipmem_size = 0x200000;
			}
			expamem_reset();
			memory_reset();
		} break;
	} //end of switch
config_changed = 1;
	check_prefs_changed_cpu();
	update_menu();
	return 0;
}

int action_main_menu(int action) {

	char name[512];

	if (action == ACTION_IDLE) {
		return 0;
	}
	switch(menu_pos) {
		case MPOS_CFG_SELECT:
			{
				RETRO_FSEL();

				if (name != NULL) {					
					strcpy(config_name, name);
					default_prefs(& changed_prefs, 0);
					changed_prefs.chipmem_size = 0;
					changed_prefs.bogomem_size = 0;
					unmount_all_hdfs();
					int ret = cfgfile_load (& changed_prefs, config_name, NULL,1,0);
					if (ret) {
						//fixup_prefs_joysticks (& changed_prefs);
						check_prefs_changed_cpu();
						check_prefs_changed_audio();
						check_prefs_changed_custom();
						uae_reset(1,1);retro_uae_reset(1);
					}
					return 1;
				} else {
					update_menu();
				}
			}; break;
		case MPOS_KICK_ROM: {

				RETRO_FSEL();

				if (name != NULL) {
					write_log("ps3: uae: kick rom open: %s\n", name);
					if (1/*uae_get_state() != UAE_STATE_STOPPED*/) {
						strcpy(changed_prefs.romfile, name);
						write_log("ps3: uae: memory_reset \n");
						memory_reset();
						write_log("ps3: uae: uae_rest(1) \n");
						uae_reset(1,1);retro_uae_reset(1);
						return 1;
					} else {
						strcpy(changed_prefs.romfile, name);
						strcpy(currprefs.romfile, name);
					}
				} else {
					write_log("ps3: uae: kick rom NULL!\n", name);
				}
				update_menu();
		} break;
		case MPOS_FLOPPY1:
		case MPOS_FLOPPY2:
		case MPOS_FLOPPY3:
		case MPOS_FLOPPY4: {

			int floppyIdx = menu_pos - MPOS_FLOPPY1;
			if (action == ACTION_CROSS) { //insert disk
				char* previousDiskName = NULL;
				if (menu_pos > 0) {
					previousDiskName = currprefs.floppyslots[floppyIdx - 1].df;
				}
				
				RETRO_FSEL();

				if (name != NULL) {
					if (currprefs.nr_floppies-1 < floppyIdx ) {
						currprefs.nr_floppies = floppyIdx  + 1;
					}
					//check whether drive is enabled
					if (currprefs.floppyslots[floppyIdx ].dfxtype < 0) {
						changed_prefs.floppyslots[floppyIdx ].dfxtype = 0;
						DISK_check_change();
					}
					strcpy (changed_prefs.floppyslots[floppyIdx ].df, name);
					DISK_check_change();
	//				disk_eject(menu_pos);
	//				disk_insert(menu_pos, name);
				}
			} else
			if (action == ACTION_SQUARE) { //eject disk
				changed_prefs.floppyslots[floppyIdx].df[0] = 0;
				DISK_check_change();
				disk_eject(floppyIdx);
			}
			update_menu();

			return 0;
			} ; break;
		case MPOS_MENU_HDF:
		case MPOS_MENU_SAVE : 
		case MPOS_MENU_CPU : 
		case MPOS_MENU_VIDEO : 
		case MPOS_MENU_SOUND : 
		case MPOS_MENU_MISC :{
			screenId = MENU_HDF + (menu_pos - MPOS_MENU_HDF);
			menu_pos = 0;
			update_menu();
		} break;

		/* RESET */
		case MPOS_RESET :{ uae_reset(0,0);retro_uae_reset(0);
			return 1;}
	}
	return 0;
}

void enter_options(void) {

	int event;
	int exit = 0;
	
	ovl_left = (currprefs.gfx_size_win.width - 600) / 2;

	if (ovl_left < 0) {
		ovl_left = (720 - 600) / 2;
	}

	memset(bmp,0,640*480*2);
	
	static first_time=0;

	if(!first_time){

		pause_sound();
		uae_pause();

		init_main_screen();
		first_time=1;
	}

	if(filebrowse==1){

		sprintf(browse_result,"%s\0",filebrowser(DEFAULT_PATH));

		if(strcmp(browse_result,"EMPTY\0")==0){
			//write_log("Cancel Fileselect(%s)\n",browse_result);
			filebrowse=0;	
		}
		else if(strcmp(browse_result,"NO CHOICE\0")==0){
			
		}
		else{
			//write_log("Ok Fileselect(%s)\n",browse_result);			
			goto fileok;			

		}

		return;

	}

	while (!exit) {

		event = update_input_gui();

		fileok:
			if(filebrowse==1)event=2;
		

		update_menu();
	
		if(event==0)event=-2;	
		else if(event!=2 && event!=3 && event!=4){	//UP/DW/LFT/RGHT SLOWDOWN

			if(idle_counter==0)sav_event=event;

			if(event!=sav_event){
				idle_counter=0;
				sav_event=event;
			}

			idle_counter++;	
			if(idle_counter<4)return;
			else {
				event=sav_event;
				idle_counter=0;
				sav_event=0;
			}

		}

		//idle event
		if (event == -2) {
			exit = do_action(10);
		} else
		if (event == PAD_DOWN) {
			menu_pos ++;
			if ( menu_pos > max_menu_pos) {
				menu_pos = 0;
			}
			update_menu();
		} else
		if (event == PAD_UP) {
			menu_pos --;
			if ( menu_pos < 0) {
				menu_pos = max_menu_pos;
			}
			update_menu();
		} else

		if (event == PAD_CROSS) {

			exit = do_action(1); 
		} else
		if (event == RETRO_X) {
			exit = do_action(2); 
		} else
		if (event == PAD_RIGHT) {
			exit = do_action(3);
		} else
		if (event == PAD_LEFT) {
			exit = do_action(4);
		}else		
		if (event == PAD_SQUARE) {
			if (screenId == MENU_MAIN) {
				exit = 1;pauseg=0;
				reset_drawing();
				first_time=0;
				uae_resume();resume_sound();
				//write_log("retro: returning from options! exit=%i \n", exit);
				return;

			} else {
				menu_pos =  (screenId - MENU_HDF) + MPOS_MENU_HDF;
				screenId = MENU_MAIN;				
				update_menu();
			}
		}		

		if(exit!=0){
			pauseg=0;
			reset_drawing();
			first_time=0;
			uae_resume();resume_sound();
		}

		return;	
	}


}


