/*
	modded for libretro-uae
*/

/*
  Hatari - dlgRom.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgRom_fileid[] = "Hatari dlgRom.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"
#include "file.h"


#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"

#define DLGROM_TOSBROWSE  4
#define DLGROM_TOSNAME    5
#define DLGROM_CARTEJECT  9
#define DLGROM_CARTBROWSE 10
#define DLGROM_CARTNAME   11
#define DLGROM_EXIT       13


/* The ROM dialog: */
static SGOBJ romdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 52,23, NULL },
	{ SGBOX, 0, 0, 1,1, 50,8, NULL },
	{ SGTEXT, 0, 0, 22,2, 9,1, "ROM setup" },
	{ SGTEXT, 0, 0, 2,5, 25,1, "Kick image:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 42,5, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 2,7, 46,1, NULL },
	{ SGBOX, 0, 0, 1,10, 50,8, NULL },

	{ SGTEXT, 0, 0, 18,11, 15,1, NULL/*"Cartridge setup" */},
	{ SGTEXT, 0, 0, 2,14, 25,1, NULL/*"Cartridge image:" */},
	{ SGTEXT/*SGBUTTON*/, 0, 0, 32,14, 8,1,  NULL/*"Eject"*/},
	{ SGTEXT/*SGBUTTON*/, 0, 0, 42,14, 8,1, NULL/*"Browse"*/ },

	{ SGTEXT, 0, 0, 2,16, 46,1, NULL },
	{ SGTEXT, 0, 0, 2,19, 25,1, "A reset is needed after changing these options." },
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 16,21, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

#ifdef LIBRETRO_PUAE
#define FILENAME_MAX 512
#endif
char szTosImageFileName[FILENAME_MAX];
bool bPatchTos;
char szCartridgeImageFileName[FILENAME_MAX];

extern int MUSTRESET;

/*-----------------------------------------------------------------------*/
/**
 * Show and process the ROM dialog.
 */
void DlgRom_Main(void)
{
	char szDlgTosName[47];
	char szDlgCartName[47];
	int but;

	SDLGui_CenterDlg(romdlg);

	File_ShrinkName(szDlgTosName, szTosImageFileName, sizeof(szDlgTosName)-1);
	romdlg[DLGROM_TOSNAME].txt = szDlgTosName;

	do
	{      
		but = SDLGui_DoDialog(romdlg, NULL);
		switch (but)
		{
		 case DLGROM_TOSBROWSE:
			/* Show and process the file selection dlg */
			SDLGui_FileConfSelect(szDlgTosName,
					      szTosImageFileName,
					      sizeof(szDlgTosName)-1,
					      false);
			break;

		}

                gui_poll_events();

	}
	while (but != DLGROM_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);

	if(szTosImageFileName!=NULL && strcmp(szTosImageFileName,"")!=0 && strcmp(szTosImageFileName,"./")!=0 ){
		printf("rom:(%s)\n",szTosImageFileName);
		strcpy(changed_prefs.romfile,szTosImageFileName );
		MUSTRESET=1;
	}

}
