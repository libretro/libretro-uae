/*
	modded for libretro-uae
*/

/*
  Hatari - dlgAbout.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  Show information about the program and its license.
*/
#include <string.h>
#include "dialog.h"
#include "sdlgui.h"

const char DlgAbout_fileid[] = "Hatari dlgAbout.c : " __DATE__ " " __TIME__;

#define PROG_NAME "Libretro P-UAE 2.6.1"

#define DLGABOUT_EXIT       17

/* The "About"-dialog: */
static SGOBJ aboutdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 48,25, NULL },
	{ SGTEXT, 0, 0, 13,1, 12,1, PROG_NAME },
	{ SGTEXT, 0, 0, 13,2, 12,1, "=============" },
	{ SGTEXT, 0, 0, 1,4, 46,1, "P-UAE LIBRETRO DEBUG VERSION" },
	{ SGTEXT, 0, 0, 1,5, 46,1, "based on P-UAE 2.6.1 https://github.com/GnoStiC/PUAE " },
	{ SGTEXT, 0, 0, 2,7, 46,1, "Git Commit: 0186c1b16f7181ffa02d73e6920d3180ce457c46" },
	{ SGTEXT, 0, 0, 1,9, 46,1, "Credits to Mustafa 'GnoStiC' TUFAN" },
	{ SGTEXT, 0, 0, 1,10, 46,1,"----------------------------------" },
	{ SGTEXT, 0, 0, 1,11, 46,1, "PUAE tries to continue where E-UAE left off.." },
	{ SGTEXT, 0, 0, 1,12, 46,1, "PUAE versioning is based on the merged WinUAE version.." },
	{ SGTEXT, 0, 0, 1,13, 46,1, "" },
	{ SGTEXT, 0, 0, 1,14, 46,1, "" },
	{ SGTEXT, 0, 0, 1,15, 46,1, "" },
	{ SGTEXT, 0, 0, 1,17, 46,1, "" },
	{ SGTEXT, 0, 0, 1,18, 46,1, "" },
	{ SGTEXT, 0, 0, 1,19, 46,1, "Gui code taken from the great HATARI emulator" },
	{ SGTEXT, 0, 0, 1,20, 46,1, "http://hatari.tuxfamily.org/"},
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 16,23, 8,1, "OK" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/*-----------------------------------------------------------------------*/
/**
 * Show the "about" dialog:
 */
void Dialog_AboutDlg(void)
{
   int but;
   /* Center PROG_NAME title string */
   aboutdlg[1].x = (aboutdlg[0].w - strlen(PROG_NAME)) / 2;

   SDLGui_CenterDlg(aboutdlg);

   do
   {
      but=SDLGui_DoDialog(aboutdlg, NULL);
      gui_poll_events();

   }
   while (but != DLGABOUT_EXIT && but != SDLGUI_QUIT
         && but != SDLGUI_ERROR && !bQuitProgram);
}
