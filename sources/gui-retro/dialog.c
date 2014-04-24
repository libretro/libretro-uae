/*
	modded for libretro-uae
*/

/*
  Hatari - dialog.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Code to handle our options dialog.
*/
const char Dialog_fileid[] = "Hatari dialog.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "memory.h"

int MUSTRESET=0;
int MUSTRESET_CFG=0;
bool bQuitProgram=false;
extern int pauseg;

/*-----------------------------------------------------------------------*/
/**
 * Open Property sheet Options dialog.
 * 
 * We keep all our configuration details in a structure called
 * 'ConfigureParams'. When we open our dialog we make a backup
 * of this structure. When the user finally clicks on 'OK',
 * we can compare and makes the necessary changes.
 * 
 * Return true if user chooses OK, or false if cancel!
 */
bool Dialog_DoProperty(void)
{
   bool bOKDialog;  /* Did user 'OK' dialog? */	
   bool bForceReset;
   bool bLoadedSnapshot;

   bQuitProgram=false;

   changed_prefs	=  currprefs;

   pause_sound();
   uae_pause();

   bOKDialog = Dialog_MainDlg(&bForceReset, &bLoadedSnapshot);

   if(bForceReset)
   {
      uae_reset(0,0);
      retro_uae_reset(0);
      return bOKDialog;
   }

   if(bQuitProgram)
   {
      uae_quit();
      retro_shutdown_uae();
   }

   if(MUSTRESET)
   {
      memory_reset();
      uae_reset(1,1);
      retro_uae_reset(1);
      MUSTRESET=0;
   }

   if(MUSTRESET_CFG)
   {
      uae_reset(1,1);
      retro_uae_reset(1);
      MUSTRESET_CFG=0;
   }

   reset_drawing();
   uae_resume();
   resume_sound();

   return bOKDialog;
}
