/*
	modded for libretro-uae
*/

/*
  Hatari - dlgMisc.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  Show information about the program and its license.
*/
const char DlgMisc_fileid[] = "Hatari dlgMisc.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"
#include "libretro-mapper.h"

#define DLGMISC_LEDON       3
#define DLGMISC_LEDOFF      4
#define DLGMISC_SPEED100    6
#define DLGMISC_SPEED200    7
#define DLGMISC_SPEED400    8
#define DLGMISC_SPEED800    9
#define DLGMISC_INC        11
#define DLGMISC_TEXT       12
#define DLGMISC_DEC        13
#define DLGMISC_SAVE       14
#define DLGMISC_LOAD       15
#define DLGMISC_EXIT       17

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "disk.h"
#ifdef LIBRETRO_FSUAE
#include "newcpu.h"
#endif
#include "autoconf.h"
#include "custom.h"
#include "inputdevice.h"
#include "memory.h"
#include "savestate.h"
#include "filesys.h"
#include "zfile.h"
#include "gensound.h"

#ifdef __PS3__
#define SAVE_PATH "/dev_hdd0/HOMEBREW/UAE/"
#else
	#ifdef AND
	#define SAVE_PATH "mnt/sdcard/euae/save/"
	#else
	#define SAVE_PATH "./save/"
	#endif

#endif

extern int stateSlot ;
extern char tmpPath[512];
static char sMaxSlot[4];
char thumbPath[512];

/* The "Misc"-dialog: */
static SGOBJ miscdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 40,25, NULL },
	{ SGTEXT, 0, 0, 13,1, 12,1, NULL },
	{ SGTEXT, 0, 0, 13,2, 12,1, "Show Leds:" },
	{ SGRADIOBUT, 0, 0, 10   ,4, 11,1, "On" },
	{ SGRADIOBUT, 0, 0, 10+11,4, 11,1, "Off" },
	{ SGTEXT, 0, 0, 13,7, 34,1, "Floppy Speed: "},
	{ SGRADIOBUT, 0, 0, 10,9, 8,1, "100" },
	{ SGRADIOBUT, 0, 0, 10,10, 8,1, "200" },
	{ SGRADIOBUT, 0, 0, 11+10,9, 8,1, "400"},
	{ SGRADIOBUT, 0, 0, 11+10,10, 8,1, "800" },
	{ SGTEXT, 0, 0, 13,12, 38,1, "States:" },
	{ SGBUTTON,   SG_EXIT/*0*/, 0, 15,14,  1,1, "\xCB" },// "\x04" },     /* Arrow left */
	{ SGTEXT,     0, 0, 17,14,  3,1,sMaxSlot },
	{ SGBUTTON,   SG_EXIT/*0*/, 0, 23,14,  1,1,  "\xCA" },//"\x03" },     /* Arrow right */
	{ SGBUTTON, SG_EXIT, 0, 13,15, 5,1, "Save" },
	{ SGBUTTON, SG_EXIT, 0, 13+5+3,15, 5,1, "Load" },

	{ SGTEXT, 0, 0, 1,20, 38,1, NULL },
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 16,23, 8,1, "OK" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

/*-----------------------------------------------------------------------*/
/**
 * Show the "about" dialog:
 */
void Dialog_MiscDlg(void)
{ 
        int but;

	SDLGui_CenterDlg(miscdlg);

	if(currprefs.leds_on_screen==1){
		miscdlg[DLGMISC_LEDON].state |= SG_SELECTED;
		miscdlg[DLGMISC_LEDOFF].state &= ~SG_SELECTED;
	}
	else {
		miscdlg[DLGMISC_LEDON].state &= ~SG_SELECTED;
		miscdlg[DLGMISC_LEDOFF].state |= SG_SELECTED;
	}

	miscdlg[DLGMISC_SPEED100].state &= ~SG_SELECTED;
	miscdlg[DLGMISC_SPEED200].state &= ~SG_SELECTED;
	miscdlg[DLGMISC_SPEED400].state &= ~SG_SELECTED;
	miscdlg[DLGMISC_SPEED800].state &= ~SG_SELECTED;

	switch(currprefs.floppy_speed){

		case 100:miscdlg[DLGMISC_SPEED100].state |= SG_SELECTED;
		break;
		case 200:miscdlg[DLGMISC_SPEED200].state |= SG_SELECTED;
		break;
		case 400:miscdlg[DLGMISC_SPEED400].state |= SG_SELECTED;
		break;
		case 800:miscdlg[DLGMISC_SPEED800].state |= SG_SELECTED;
		break;
	}

	sprintf(sMaxSlot, "%3i", stateSlot);

        do
	{

		
                but=SDLGui_DoDialog(miscdlg, NULL);

		sprintf(thumbPath, "%sstate%03d.img\0", SAVE_PATH, stateSlot);
 		loadthumb(thumbPath,256,300);

		switch(but){

		 case DLGMISC_INC:
				stateSlot++;if(stateSlot>100)stateSlot=1;
				sprintf(sMaxSlot, "%3i", stateSlot);
			break;
		 case DLGMISC_DEC:
				stateSlot--;if(stateSlot<1)stateSlot=100;
				sprintf(sMaxSlot, "%3i", stateSlot);
			break;

		 case DLGMISC_LOAD:

				sprintf(tmpPath, "%sstate%03d.asf\0", SAVE_PATH, stateSlot);
				if (fsel_file_exists(tmpPath)) {
					savestate_initsave (tmpPath, 0, 0, 0);
					//savestate_initsave (tmpPath, 1,1,1);
					savestate_state = STATE_DORESTORE;

					return;
				}

			break;
		 case DLGMISC_SAVE:

				sprintf(tmpPath, "%sstate%03d.asf\0", SAVE_PATH, stateSlot);
				sprintf(thumbPath, "%sstate%03d.img\0", SAVE_PATH, stateSlot);
				dothumb(thumbPath,640,480,128,96);

				if (!fsel_file_exists(tmpPath)) {
					fsel_file_create(tmpPath);
				}
				savestate_initsave (tmpPath, 0, 0, 0);
				save_state (tmpPath, "puae");
				//savestate_initsave (tmpPath, 1);
				//save_state (tmpPath, "p-uae");

			break;


		}

                gui_poll_events();

        }
        while (but != DLGMISC_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);


	if(miscdlg[DLGMISC_LEDON].state& SG_SELECTED){
		currprefs.leds_on_screen = 1;
	}
	else if(miscdlg[DLGMISC_LEDOFF].state& SG_SELECTED){
		currprefs.leds_on_screen = 0;
		clean_led_area();
	}

	if(miscdlg[DLGMISC_SPEED100].state & SG_SELECTED)changed_prefs.floppy_speed=100;
	else if(miscdlg[DLGMISC_SPEED200].state & SG_SELECTED)changed_prefs.floppy_speed=200;
	else if(miscdlg[DLGMISC_SPEED400].state & SG_SELECTED)changed_prefs.floppy_speed=400;
	else if(miscdlg[DLGMISC_SPEED800].state & SG_SELECTED)changed_prefs.floppy_speed=800;
	DISK_check_change();

}
