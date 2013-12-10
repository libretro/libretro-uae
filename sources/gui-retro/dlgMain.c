/*
	modded for libretro-uae
*/

/*
  Hatari - dlgMain.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  The main dialog.
*/
const char DlgMain_fileid[] = "Hatari dlgMain.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"

#define MAINDLG_ABOUT    2
#define MAINDLG_SYSTEM   3
#define MAINDLG_ROM      4
#define MAINDLG_MEMORY   5
#define MAINDLG_FLOPPYS  6
#define MAINDLG_HARDDISK 7
#define MAINDLG_VIDEO    8
#define MAINDLG_WINDOW   9
#define MAINDLG_MISC     10
#define MAINDLG_JOYSTICK 11
#define MAINDLG_DEVICES  12
#define MAINDLG_SOUND    13
#define MAINDLG_LOADCFG  14
#define MAINDLG_SAVECFG  15
#define MAINDLG_NORESET  16
#define MAINDLG_RESET    17
#define MAINDLG_OK       18
#define MAINDLG_QUIT     19
#define MAINDLG_CANCEL   20


/* The main dialog: */
static SGOBJ maindlg[] =
{
	{ SGBOX, 0, 0, 0,0, 50,19, NULL },
	{ SGTEXT, 0, 0, 17,1, 16,1, "P-UAE main menu" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 2,4, 13,1, "About" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 2,6, 13,1, "System" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 2,8, 13,1, "ROM" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 2,10, 13,1, "Memory" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 17,4, 16,1, "Floppy disks" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 17,6, 16,1, "Hard disks" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 17,8, 16,1, "Video" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 17,10, 16,1, "Hatari screen" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 35,4, 13,1, "Misc" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 35,6, 13,1, "Joystick" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 35,8, 13,1, "Devices" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 35,10, 13,1, "Sound" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 7,13, 16,1, "Load config." },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 27,13, 16,1, "Save config." },
	{ SGRADIOBUT, 0, 0, 3,15, 15,1, "No Reset" },
	{ SGRADIOBUT, 0, 0, 3,17, 15,1, "Reset machine" },
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 21,15, 8,3, "OK" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 36,15, 10,1, "Quit" },
	{ SGBUTTON, SG_EXIT/*SG_CANCEL*/, 0, 36,17, 10,1, "Cancel" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

char sConfigFileName[512];
extern int MUSTRESET_CFG;

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"

/**
 * This functions sets up the actual font and then displays the main dialog.
 */
int Dialog_MainDlg(bool *bReset, bool *bLoadedSnapshot)
{
	int retbut=0;
	bool bOldMouseVisibility;
	int nOldMouseX, nOldMouseY;
	char *psNewCfg;

	*bReset = false;
	*bLoadedSnapshot = false;

	if (SDLGui_SetScreen(/*sdlscrn*/))
		return false;

	SDLGui_CenterDlg(maindlg);

	maindlg[MAINDLG_NORESET].state |= SG_SELECTED;
	maindlg[MAINDLG_RESET].state &= ~SG_SELECTED;

	do
	{       
		retbut = SDLGui_DoDialog(maindlg, NULL);
		switch (retbut)
		{
		 case MAINDLG_ABOUT:
			Dialog_AboutDlg();
			break;

		 case MAINDLG_FLOPPYS:
			DlgFloppy_Main();
			break;

		 case MAINDLG_HARDDISK:
			DlgHardDisk_Main();
			break;

		 case MAINDLG_ROM:
			DlgRom_Main();
			break;

		 case MAINDLG_VIDEO:
			Dialog_VideoDlg();
			break;

		 case MAINDLG_SYSTEM:
			Dialog_SystemDlg();
			break;

		 case MAINDLG_MEMORY:
			if (Dialog_MemDlg())
			{
				/* Memory snapshot has been loaded - leave GUI immediately */
				*bLoadedSnapshot = true;
				//Main_WarpMouse(nOldMouseX, nOldMouseY);
				return true;
			}
			break;

		 case MAINDLG_MISC:
			Dialog_MiscDlg();
			break;

		 case MAINDLG_JOYSTICK:
			Dialog_JoyDlg();
			break;

		 case MAINDLG_SOUND:
			Dialog_SoundDlg();
			break;

		 case MAINDLG_LOADCFG:
			strcpy(sConfigFileName, ".uaerc");
			psNewCfg = SDLGui_FileSelect(sConfigFileName, NULL, false);
			if (psNewCfg)
			{					
					default_prefs(& changed_prefs, 0);
					changed_prefs.chipmem_size = 0;
					changed_prefs.bogomem_size = 0;
					unmount_all_hdfs();
					int ret = cfgfile_load (& changed_prefs, psNewCfg, NULL,1,0);
					if (ret) {
						
						check_prefs_changed_cpu();
						check_prefs_changed_audio();
						check_prefs_changed_custom();
						
						MUSTRESET_CFG=1;
					}
					

				strcpy(sConfigFileName, psNewCfg);

			}
			break;

		 case MAINDLG_QUIT:
			bQuitProgram = true;
			break;
		}
                gui_poll_events();
	}
	while (retbut != MAINDLG_OK && retbut != MAINDLG_CANCEL && retbut != SDLGUI_QUIT
	        && retbut != SDLGUI_ERROR && !bQuitProgram);


	if (maindlg[MAINDLG_RESET].state & SG_SELECTED)
		*bReset = true;

	return (retbut == MAINDLG_OK);
}
