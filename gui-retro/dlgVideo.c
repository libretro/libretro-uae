/*
	modded for libretro-uae
*/

/*
  Hatari - dlgVideo.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  Show information about the program and its license.
*/
const char DlgVideo_fileid[] = "Hatari dlgVideo.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"

#define DLGVIDEO_CHIPOCS	3
#define DLGVIDEO_CHIPECSAGNUS 	4
#define DLGVIDEO_CHIPECSDENISE	5
#define DLGVIDEO_CHIPECSFULL	6
#define DLGVIDEO_CHIPAGA	7
#define DLGVIDEO_NTSC		8
#define DLGVIDEO_COLLNONE	10
#define DLGVIDEO_COLLSPRT	11
#define DLGVIDEO_COLLPLAYF	12
#define DLGVIDEO_COLLFULL	13
#define DLGVIDEO_VSYNC		14
#define DLGVIDEO_BLITS		15
#define DLGVIDEO_BLITCYCLE	16
#define DLGVIDEO_FSKIP		18
#define DLGVIDEO_EXIT       20

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


/* The "Video"-dialog: */
static SGOBJ videodlg[] =
{
	{ SGBOX, 0, 0, 0,0, 40,25, NULL },				//0
	{ SGTEXT, 0, 0, 13,1, 12,1, NULL },				//1

	{ SGTEXT, 0, 0, 13,2, 12,1, "Chipset type:" },			//2
	{ SGRADIOBUT, 0, 0, 10   ,4, 11,1, "OCS" },			//3
	{ SGRADIOBUT, 0, 0, 10   ,5, 11,1, "ECS AGNUS" },		//4
	{ SGRADIOBUT, 0, 0, 10   ,6, 11,1, "ECS DENISE" },		//5
	{ SGRADIOBUT, 0, 0, 10+11,4, 11,1, "ECS FULL" },		//6
	{ SGRADIOBUT, 0, 0, 10+11,5, 11,1, "AGA" },			//7
	{ SGCHECKBOX, 0, 0, 10+11,6, 11,1, "NTSC" },			//8

	{ SGTEXT, 0, 0, 13,8, 34,1, "Collision Level: "},		//9
	{ SGRADIOBUT, 0, 0, 10,10, 8,1, "None" },			//10
	{ SGRADIOBUT, 0, 0, 10,11, 8,1, "Sprites" },			//11
	{ SGRADIOBUT, 0, 0, 11+10,10, 8,1, "Playfields"},		//12
	{ SGRADIOBUT, 0, 0, 11+10,11, 8,1, "Full" },			//13

	{ SGCHECKBOX, 0, 0, 4+11,13, 11,1, "VSYNC" },			//14
	{ SGCHECKBOX, 0, 0, 4+11,14, 17,1, "Immediate Blit" },		//15
	{ SGCHECKBOX, 0, 0, 4+11,15, 22,1, "Cycle Exact Blit" },	//16

	{ SGTEXT, 0, 0, 1,16, 38,1, NULL },				//17
	{ SGTEXT, 0, 0, 1,17, 38,1, NULL },				//18
	{ SGTEXT, 0, 0, 1,18, 38,1, NULL },				//19
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 19,23, 8,1, "OK" },	//20
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/*-----------------------------------------------------------------------*/
/**
 * Show the "about" dialog:
 */
void Dialog_VideoDlg(void)
{ 
        int but;

	SDLGui_CenterDlg(videodlg);

	videodlg[DLGVIDEO_CHIPOCS].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_CHIPECSAGNUS].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_CHIPECSDENISE].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_CHIPECSFULL].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_CHIPAGA].state &= ~SG_SELECTED;

	if (currprefs.chipset_mask & CSMASK_AGA)
		videodlg[DLGVIDEO_CHIPAGA].state |= SG_SELECTED;
	else if ((currprefs.chipset_mask & CSMASK_ECS_AGNUS) && (currprefs.chipset_mask & CSMASK_ECS_DENISE))
		videodlg[DLGVIDEO_CHIPECSFULL].state |= SG_SELECTED;
	else if (currprefs.chipset_mask & CSMASK_ECS_AGNUS)
		videodlg[DLGVIDEO_CHIPECSAGNUS].state |= SG_SELECTED;
	else if (currprefs.chipset_mask & CSMASK_ECS_DENISE)
		videodlg[DLGVIDEO_CHIPECSDENISE].state |= SG_SELECTED;
	else videodlg[DLGVIDEO_CHIPOCS].state |= SG_SELECTED;

	if (currprefs.ntscmode==1)videodlg[DLGVIDEO_NTSC].state |= SG_SELECTED;
	else videodlg[DLGVIDEO_NTSC].state &= ~SG_SELECTED;

	videodlg[DLGVIDEO_COLLNONE].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_COLLSPRT].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_COLLPLAYF].state &= ~SG_SELECTED;
	videodlg[DLGVIDEO_COLLFULL].state &= ~SG_SELECTED;

	if (currprefs.collision_level==0)
		videodlg[DLGVIDEO_COLLNONE].state |= SG_SELECTED;
	else if (currprefs.collision_level==1)
		videodlg[DLGVIDEO_COLLSPRT].state |= SG_SELECTED;
	else if (currprefs.collision_level==2)
		videodlg[DLGVIDEO_COLLPLAYF].state |= SG_SELECTED;
	else if (currprefs.collision_level==3)
		videodlg[DLGVIDEO_COLLFULL].state |= SG_SELECTED;

	if (currprefs.gfx_apmode[0].gfx_vsync==1)videodlg[DLGVIDEO_VSYNC].state |= SG_SELECTED;
	else videodlg[DLGVIDEO_VSYNC].state &= ~SG_SELECTED;

	if (currprefs.immediate_blits==1)videodlg[DLGVIDEO_BLITS].state |= SG_SELECTED;
	else videodlg[DLGVIDEO_BLITS].state &= ~SG_SELECTED;

	if (currprefs.cpu_cycle_exact==1)videodlg[DLGVIDEO_BLITCYCLE].state |= SG_SELECTED;
	else videodlg[DLGVIDEO_BLITCYCLE].state &= ~SG_SELECTED;

        do
	{
                but=SDLGui_DoDialog(videodlg, NULL);
                gui_poll_events();

        }
        while (but != DLGVIDEO_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);


	if(videodlg[DLGVIDEO_CHIPOCS].state & SG_SELECTED){
		changed_prefs.chipset_mask = 0;
	}
	else if(videodlg[DLGVIDEO_CHIPECSAGNUS].state & SG_SELECTED){
		changed_prefs.chipset_mask = 1; 
	}
	else if(videodlg[DLGVIDEO_CHIPECSDENISE].state & SG_SELECTED){
		changed_prefs.chipset_mask = 2;
	}
	else if(videodlg[DLGVIDEO_CHIPECSFULL].state & SG_SELECTED){
		changed_prefs.chipset_mask = 3;
	}
	else if(videodlg[DLGVIDEO_CHIPAGA].state & SG_SELECTED){
		changed_prefs.chipset_mask = 4;
	}

	if(videodlg[DLGVIDEO_NTSC].state& SG_SELECTED){
		changed_prefs.ntscmode = 1;
	}
	else {
		changed_prefs.ntscmode = 0;
	}

	if(videodlg[DLGVIDEO_COLLNONE].state & SG_SELECTED){
		changed_prefs.collision_level = 0;
	}
	else if(videodlg[DLGVIDEO_COLLSPRT].state & SG_SELECTED){
		changed_prefs.collision_level = 1; 
	}
	else if(videodlg[DLGVIDEO_COLLPLAYF].state & SG_SELECTED){
		changed_prefs.collision_level = 2;
	}
	else if(videodlg[DLGVIDEO_COLLFULL].state & SG_SELECTED){
		changed_prefs.collision_level = 3;
	}

	if(videodlg[DLGVIDEO_VSYNC].state& SG_SELECTED){
		changed_prefs.gfx_apmode[0].gfx_vsync = 1;
	}
	else {
		changed_prefs.gfx_apmode[0].gfx_vsync = 0;
	}

	if(videodlg[DLGVIDEO_BLITS].state& SG_SELECTED){
		changed_prefs.immediate_blits = 1;
	}
	else {
		changed_prefs.immediate_blits = 0;
	}

	if(videodlg[DLGVIDEO_BLITCYCLE].state& SG_SELECTED){
		changed_prefs.cpu_cycle_exact = 1;
	}
	else {
		changed_prefs.cpu_cycle_exact = 0;
	}



	config_changed = 1;
	check_prefs_changed_custom();

}
