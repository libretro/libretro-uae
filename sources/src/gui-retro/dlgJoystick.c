/*
	modded for libretro-uae
*/

/*
  Hatari - dlgJoystick.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgJoystick_fileid[] = "Hatari dlgJoystick.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "inputdevice.h"

#define DLGJOY_MSJY  3
#define DLGJOY_JYJY  4  
#define DLGJOY_MSMS  5
#define DLGJOY_EXIT  6

/* The joysticks dialog: */

static char sSdlStickName[20];

static SGOBJ joydlg[] =
{
	{ SGBOX, 0, 0, 0,0, 32,18, NULL },
	{ SGTEXT, 0, 0, 8,1, 15,1, "Joysticks setup" },

	{ SGBOX, 0, 0, 1,4, 30,11, NULL },
	{ SGRADIOBUT, 0, 0, 2,5, 10,1, "Mouse & Joy1" },
	{ SGRADIOBUT, 0, 0, 2,6, 20,1, "Joy1 & Joy2" },
	{ SGRADIOBUT, 0, 0, 2,7, 14,1, "Mouse & Mouse" },


	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 6,16, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

/*-----------------------------------------------------------------------*/
/**
 * Show and process the joystick dialog.
 */
void Dialog_JoyDlg(void)
{
	int but;

	SDLGui_CenterDlg(joydlg);

	joydlg[DLGJOY_MSJY].state &= ~SG_SELECTED;
	joydlg[DLGJOY_JYJY].state &= ~SG_SELECTED;
	joydlg[DLGJOY_MSMS].state &= ~SG_SELECTED;

	int j0 = currprefs.jports[0].id/ JSEM_JOYS; 
	int j1 = currprefs.jports[1].id/ JSEM_JOYS;

	if (j0 == 2 && j1 == 1) { //mouse & joy-1
		joydlg[DLGJOY_MSJY].state |= SG_SELECTED;
	}
	else if (j0 == 1 && j1 == 1) { //joy-2 & joy-1
		joydlg[DLGJOY_JYJY].state |= SG_SELECTED;
	}
	else {
		joydlg[DLGJOY_MSMS].state |= SG_SELECTED;
	}

	do
	{
    		but = SDLGui_DoDialog(joydlg, NULL);

 		gui_poll_events();
	}
	while (but != DLGJOY_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);
	

	if(joydlg[DLGJOY_MSJY].state & SG_SELECTED){
		changed_prefs.jports[0].id = JSEM_MICE;
		changed_prefs.jports[1].id = JSEM_JOYS;
		changed_prefs.mouse_settings[1]->enabled = 0;
	}
	else if(joydlg[DLGJOY_JYJY].state & SG_SELECTED){
		changed_prefs.jports[0].id = JSEM_JOYS + 1;
		changed_prefs.jports[1].id = JSEM_JOYS;
		changed_prefs.mouse_settings[1]->enabled = 0;
	}
	else if(joydlg[DLGJOY_MSMS].state & SG_SELECTED){
		changed_prefs.jports[0].id = JSEM_MICE;
		changed_prefs.jports[1].id = JSEM_MICE + 1;
		changed_prefs.mouse_settings[1]->enabled = 1;
	}

	inputdevice_config_change();
	check_prefs_changed_custom();


}

