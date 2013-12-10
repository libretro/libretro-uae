/*
	modded for libretro-uae
*/

/*
  Hatari - dlgHardDisk.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgHardDisk_fileid[] = "Hatari dlgHardDisk.c : " __DATE__ " " __TIME__;

#include <assert.h>

#include "dialog.h"
#include "sdlgui.h"
#include "file.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "disk.h"

  bool bUseHardDiskDirectories;
  bool bUseHardDiskImage[6];
  bool bUseIdeMasterHardDiskImage;
  bool bUseIdeSlaveHardDiskImage;

  char szHardDiskImage[6][FILENAME_MAX];

#define DISKDLG_HD0EJECT          3
#define DISKDLG_HD0BROWSE         4
#define DISKDLG_ACSINAME           5
#define DISKDLG_HD1EJECT     7
#define DISKDLG_HD1BROWSE    8
#define DISKDLG_IDEMASTERNAME      9
#define DISKDLG_HD2EJECT     11
#define DISKDLG_HD2BROWSE    12
#define DISKDLG_IDESLAVENAME      13
#define DISKDLG_HD3EJECT       15
#define DISKDLG_HD3BROWSE      16
#define DISKDLG_GEMDOSNAME        17
#define DISKDLG_PROTOFF           19
#define DISKDLG_PROTON            20
#define DISKDLG_PROTAUTO          21
#define DISKDLG_BOOTHD            22
#define DISKDLG_EXIT              23


/* The disks dialog: */
static SGOBJ diskdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 64,19, NULL },
	{ SGTEXT, 0, 0, 27,1, 10,1, "Hard disks" },

	{ SGTEXT, 0, 0, 2,3, 14,1, "HD image 1:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,3, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,3, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,4, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,5, 20,1, "HD image 2:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,5, 7,1, "Eject" },
	{ SGBUTTON,SG_EXIT/*0*/, 0, 54,5, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,6, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,7, 19,1, "HD image 3:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,7, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,7, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,8, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,9, 13,1, "HD image 4:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,9, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,9, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,10, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,12, 31,1, "GEMDOS drive write protection:" },
	{ SGRADIOBUT, 0, 0, 33,12, 5,1, "Off" },
	{ SGRADIOBUT, 0, 0, 40,12, 5,1, "On" },
	{ SGRADIOBUT, 0, 0, 46,12, 6,1, "Auto" },

	{ SGCHECKBOX, 0, 0, 2,14, 14,1, "Boot from HD" },

	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 22,16, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/**
 * Let user browse given directory, set directory if one selected.
 * return false if none selected, otherwise return true.
 */
static bool DlgDisk_BrowseDir(char *dlgname, char *confname, int maxlen)
{
	char *str, *selname;

	selname = SDLGui_FileSelect(confname, NULL, false);
	if (selname)
	{
		strcpy(confname, selname);
		free(selname);

		str = strrchr(confname, PATHSEP);
		if (str != NULL)
			str[1] = 0;
		File_CleanFileName(confname);
		File_ShrinkName(dlgname, confname, maxlen);
		return true;
	}
	return false;
}


/**
 * Show and process the hard disk dialog.
 */
void DlgHardDisk_Main(void)
{
	int but, i;
	char dlgname_gdos[64], dlgname_acsi[64];
	char dlgname_ide_master[64], dlgname_ide_slave[64];

	SDLGui_CenterDlg(diskdlg);

	/* Set up dialog to actual values: */

	/* Boot from harddisk? */

	/* ACSI hard disk image: */
	if (bUseHardDiskImage[0])
		File_ShrinkName(dlgname_acsi, szHardDiskImage[0],
		                diskdlg[DISKDLG_ACSINAME].w);
	else
		dlgname_acsi[0] = '\0';
	diskdlg[DISKDLG_ACSINAME].txt = dlgname_acsi;


	/* IDE master hard disk image: */
	if (bUseHardDiskImage[1])
		File_ShrinkName(dlgname_ide_master, szHardDiskImage[1],
		                diskdlg[DISKDLG_IDEMASTERNAME].w);
	else
		dlgname_ide_master[0] = '\0';
	diskdlg[DISKDLG_IDEMASTERNAME].txt = dlgname_ide_master;

	/* IDE slave hard disk image: */
	if (bUseHardDiskImage[2])
		File_ShrinkName(dlgname_ide_slave, szHardDiskImage[2],
		                diskdlg[DISKDLG_IDESLAVENAME].w);
	else
		dlgname_ide_slave[0] = '\0';
	diskdlg[DISKDLG_IDESLAVENAME].txt = dlgname_ide_slave;

	/* GEMDOS hard disk directory: */
	if (bUseHardDiskImage[3])
		File_ShrinkName(dlgname_gdos, szHardDiskImage[4],
		                diskdlg[DISKDLG_GEMDOSNAME].w);
	else
		dlgname_gdos[0] = '\0';
	diskdlg[DISKDLG_GEMDOSNAME].txt = dlgname_gdos;


	/* Draw and process the dialog */
	do
	{       
		but = SDLGui_DoDialog(diskdlg, NULL);
		switch (but)
		{
		 case DISKDLG_HD0EJECT:
			bUseHardDiskImage[0] = false;
			dlgname_acsi[0] = '\0';

			if(szHardDiskImage[0]!=0)
			kill_filesys_unitconfig (&currprefs, 0);
			break;
		 case DISKDLG_HD0BROWSE:
			if (SDLGui_FileConfSelect(dlgname_acsi,
			                          szHardDiskImage[0],
			                          diskdlg[DISKDLG_ACSINAME].w, false)){
				
				bUseHardDiskImage[0] = true;
				mount_hdf(0,szHardDiskImage[0]);
				}
			break;

		 case DISKDLG_HD1EJECT:
			bUseHardDiskImage[1] = false;
			dlgname_ide_master[0] = '\0';

			if(szHardDiskImage[1]!=0)
			kill_filesys_unitconfig (&currprefs, 1);

			break;
		 case DISKDLG_HD1BROWSE:
			if (SDLGui_FileConfSelect(dlgname_ide_master,
			                          szHardDiskImage[1],
			                          diskdlg[DISKDLG_IDEMASTERNAME].w, false)){
				bUseHardDiskImage[1] = true;
				mount_hdf(1,szHardDiskImage[1]);}
			break;

		 case DISKDLG_HD2EJECT:
			bUseHardDiskImage[2]= false;
			dlgname_ide_slave[0] = '\0';
			if(szHardDiskImage[2]!=0)
			kill_filesys_unitconfig (&currprefs, 2);

			break;
		 case DISKDLG_HD2BROWSE:
			if (SDLGui_FileConfSelect(dlgname_ide_slave,
			                          szHardDiskImage[2],
			                          diskdlg[DISKDLG_IDESLAVENAME].w, false)){
				bUseHardDiskImage[2] = true;
				mount_hdf(2,szHardDiskImage[2]);}
			break;
		 case DISKDLG_HD3EJECT:
			bUseHardDiskImage[3]= false;
			dlgname_gdos[0] = '\0';
			if(szHardDiskImage[3]!=0)
			kill_filesys_unitconfig (&currprefs, 3);
			break;

		 case DISKDLG_HD3BROWSE:
			if (DlgDisk_BrowseDir(dlgname_gdos,
			                     szHardDiskImage[3],
			                     diskdlg[DISKDLG_GEMDOSNAME].w)){
				bUseHardDiskImage[3]= true;
				mount_hdf(3,szHardDiskImage[3]);}
			break;


		}
                gui_poll_events();
	}
	while (but != DISKDLG_EXIT && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram);

}
