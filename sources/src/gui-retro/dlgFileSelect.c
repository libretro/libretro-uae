/*
	modded for libretro-uae
*/

/*
  Hatari - dlgFileSelect.c
 
  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
 
  A file selection dialog for the graphical user interface for Hatari.
*/
const char DlgFileSelect_fileid[] = "Hatari dlgFileSelect.c : " __DATE__ " " __TIME__;

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdlgui.h"
#include "file.h"
#include "paths.h"
#include "zip.h"
#include "dialog.h"
#include "libretro-mapper.h"

//RETRO
extern int alphasort2(const struct dirent **d1, const struct dirent **d2);
/*
int alphasort2(const struct dirent **d1, const struct dirent **d2)
{
	    const struct dirent *c1;
	    const struct dirent *c2;
	    c1 = *d1;
	    c2 = *d2;
	    return strcmp(c1->d_name, c2->d_name);
}
*/
#define SGFS_NUMENTRIES   16            /* How many entries are displayed at once */


#define SGFSDLG_FILENAME    5
#define SGFSDLG_UPDIR       6
#define SGFSDLG_CWD         7
#define SGFSDLG_HOMEDIR     8
#define SGFSDLG_ROOTDIR     9
#define SGFSDLG_ENTRYFIRST 12
#define SGFSDLG_ENTRYLAST  27
#define SGFSDLG_SCROLLBAR  28
#define SGFSDLG_UP         29
#define SGFSDLG_DOWN       30
#define SGFSDLG_SHOWHIDDEN 31
#define SGFSDLG_OKAY       32
#define SGFSDLG_CANCEL     33

#define SCROLLOUT_ABOVE  1
#define SCROLLOUT_UNDER  2

#define DLGPATH_SIZE 62
static char dlgpath[DLGPATH_SIZE+1];    /* Path name in the dialog */

#define DLGFNAME_SIZE 56
static char dlgfname[DLGFNAME_SIZE+1];  /* Name of the selected file in the dialog */

#define DLGFILENAMES_SIZE 59
static char dlgfilenames[SGFS_NUMENTRIES][DLGFILENAMES_SIZE+1];  /* Visible file names in the dialog */

#define SCROLLBAR_MIN_HEIGHT 4		/* Min value for yScrollbar_size */

/* The dialog data: */
static SGOBJ fsdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 64,25, NULL },
	{ SGTEXT, 0, 0, 25,1, 13,1, "Choose a file" },
	{ SGTEXT, 0, 0, 1,2, 7,1, "Folder:" },
	{ SGTEXT, 0, 0, 1,3, DLGPATH_SIZE,1, dlgpath },
	{ SGTEXT, 0, 0, 1,4, 6,1, "File:" },
	{ SGTEXT, 0, 0, 7,4, DLGFNAME_SIZE,1, dlgfname },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 45,1, 4,1, ".." },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 50,1, 5,1, "CWD" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 56,1, 3,1, "~" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 60,1, 3,1, "/" },
	{ SGBOX, 0, 0, 1,6, 62,16, NULL },
	{ SGBOX, 0, 0, 62,7, 1,14, NULL },
	{ SGTEXT, SG_EXIT, 0, 2,6, DLGFILENAMES_SIZE,1, dlgfilenames[0] },
	{ SGTEXT, SG_EXIT, 0, 2,7, DLGFILENAMES_SIZE,1, dlgfilenames[1] },
	{ SGTEXT, SG_EXIT, 0, 2,8, DLGFILENAMES_SIZE,1, dlgfilenames[2] },
	{ SGTEXT, SG_EXIT, 0, 2,9, DLGFILENAMES_SIZE,1, dlgfilenames[3] },
	{ SGTEXT, SG_EXIT, 0, 2,10, DLGFILENAMES_SIZE,1, dlgfilenames[4] },
	{ SGTEXT, SG_EXIT, 0, 2,11, DLGFILENAMES_SIZE,1, dlgfilenames[5] },
	{ SGTEXT, SG_EXIT, 0, 2,12, DLGFILENAMES_SIZE,1, dlgfilenames[6] },
	{ SGTEXT, SG_EXIT, 0, 2,13, DLGFILENAMES_SIZE,1, dlgfilenames[7] },
	{ SGTEXT, SG_EXIT, 0, 2,14, DLGFILENAMES_SIZE,1, dlgfilenames[8] },
	{ SGTEXT, SG_EXIT, 0, 2,15, DLGFILENAMES_SIZE,1, dlgfilenames[9] },
	{ SGTEXT, SG_EXIT, 0, 2,16, DLGFILENAMES_SIZE,1, dlgfilenames[10] },
	{ SGTEXT, SG_EXIT, 0, 2,17, DLGFILENAMES_SIZE,1, dlgfilenames[11] },
	{ SGTEXT, SG_EXIT, 0, 2,18, DLGFILENAMES_SIZE,1, dlgfilenames[12] },
	{ SGTEXT, SG_EXIT, 0, 2,19, DLGFILENAMES_SIZE,1, dlgfilenames[13] },
	{ SGTEXT, SG_EXIT, 0, 2,20, DLGFILENAMES_SIZE,1, dlgfilenames[14] },
	{ SGTEXT, SG_EXIT, 0, 2,21, DLGFILENAMES_SIZE,1, dlgfilenames[15] },
	{ SGSCROLLBAR, SG_TOUCHEXIT, 0, 62, 7, 0, 0, NULL },       /* Scrollbar */
	{ SGBUTTON, SG_TOUCHEXIT, 0, 62, 6,1,1, "\xCB"/*"\x01"*/  },          /* Arrow up */
	{ SGBUTTON, SG_TOUCHEXIT, 0, 62,21,1,1, "\xCA"/*"\x02"*/  },          /* Arrow down */
	{ SGCHECKBOX, SG_EXIT, 0, 2,23, 18,1, "Show hidden files" },
	{ SGBUTTON, SG_EXIT /*SG_DEFAULT*/, 0, 32,23, 8,1, "Okay" },
	{ SGBUTTON, SG_EXIT/* SG_CANCEL*/, 0, 50,23, 8,1, "Cancel" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


static int ypos = -1;				/* First entry number to be displayed. If -1, file selector start on the 1st file */
						/* else we continue from the previous position when SDLGui_FileSelect is called again */
static bool refreshentries;			/* Do we have to update the file names in the dialog? */
static int entries;				/* How many files are in the actual directory? */
static int oldMouseY = 0;			/* Keep the latest Y mouse position for scrollbar move computing */
static int mouseClicked = 0;			/* used to know if mouse if down for the first time or not */
static int mouseIsOut = 0;			/* used to keep info that mouse if above or under the scrollbar when mousebutton is down */
static float scrollbar_Ypos = 0.0;		/* scrollbar heigth */

/* Convert file position (in file list) to scrollbar y position */
static void DlgFileSelect_Convert_ypos_to_scrollbar_Ypos(void);



/*-----------------------------------------------------------------------*/
/**
 * Update the file name strings in the dialog.
 * Returns false if it failed, true on success.
 */
static int DlgFileSelect_RefreshEntries(struct dirent **files, char *path, bool browsingzip)
{
	int i;
	char *tempstr = malloc(FILENAME_MAX);

	if (!tempstr)
	{
		perror("DlgFileSelect_RefreshEntries");
		return false;
	}

	/* Copy entries to dialog: */
	for (i=0; i<SGFS_NUMENTRIES; i++)
	{
		if (i+ypos < entries)
		{
			struct stat filestat;
			/* Prepare entries: */
			strcpy(tempstr, "  ");
			strcat(tempstr, files[i+ypos]->d_name);
			File_ShrinkName(dlgfilenames[i], tempstr, DLGFILENAMES_SIZE);
			/* Mark folders: */
			strcpy(tempstr, path);
			strcat(tempstr, files[i+ypos]->d_name);

			if (browsingzip)
			{
				if (File_DoesFileNameEndWithSlash(tempstr))
					dlgfilenames[i][0] = SGFOLDER;    /* Mark folders */
			}
			else
			{
				if( stat(tempstr, &filestat)==0 && S_ISDIR(filestat.st_mode) )
					dlgfilenames[i][0] = SGFOLDER;    /* Mark folders */
				if (ZIP_FileNameIsZIP(tempstr) && browsingzip == false)
					dlgfilenames[i][0] = SGFOLDER;    /* Mark .ZIP archives as folders */
			}
		}
		else
			dlgfilenames[i][0] = 0;  /* Clear entry */
	}

	free(tempstr);
	return true;
}


/*-----------------------------------------------------------------------*/
/**
 * Remove all hidden files (files with file names that begin with a dot) from
 * the list.
 */
static void DlgFileSelect_RemoveHiddenFiles(struct dirent **files)
{
	int i;
	int nActPos = -1;
	int nOldEntries;

	nOldEntries = entries;

	/* Scan list for hidden files and remove them. */
	for (i = 0; i < nOldEntries; i++)
	{
		/* Does file name start with a dot? -> hidden file! */
		if (files[i]->d_name[0] == '.')
		{
			if (nActPos == -1)
				nActPos = i;
			/* Remove file from list: */
			free(files[i]);
			files[i] = NULL;
			entries -= 1;
		}
	}

	/* Now close the gaps in the list: */
	if (nActPos != -1)
	{
		for (i = nActPos; i < nOldEntries; i++)
		{
			if (files[i] != NULL)
			{
				/* Move entry to earlier position: */
				files[nActPos] = files[i];
				files[i] = NULL;
				nActPos += 1;
			}
		}
	}
}


/*-----------------------------------------------------------------------*/
/**
 * Prepare to scroll up one entry.
 */
static void DlgFileSelect_ScrollUp(void)
{
	if (ypos > 0)
	{
		--ypos;
		DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
		refreshentries = true;
	}
}


/*-----------------------------------------------------------------------*/
/**
 * Prepare to scroll down one entry.
 */
static void DlgFileSelect_ScrollDown(void)
{
	if (ypos+SGFS_NUMENTRIES < entries)
	{
		++ypos;
		DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
		refreshentries = true;
	}
}

//RETRO
extern int gmx,gmy;

/*-----------------------------------------------------------------------*/
/**
 * Manage the scrollbar up or down.
 */
static void DlgFileSelect_ManageScrollbar(void)
{
	int b, y;
	int scrollY, scrollYmin, scrollYmax, scrollH_half;
	float scrollMove;
	
	y=gmy;//b = SDL_GetMouseState(&x, &y);

	/* If mouse is down on the scrollbar for the first time */
	if (fsdlg[SGFSDLG_SCROLLBAR].state & SG_MOUSEDOWN) {
		if (mouseClicked == 0) {
			mouseClicked = 1;
			mouseIsOut = 0;
			oldMouseY = y;
		}
	}
	/* Mouse button is up on the scrollbar */
	else {
		mouseClicked = 0;
		oldMouseY = y;
		mouseIsOut = 0;
	}
 	
	/* If mouse Y position didn't change */ 
	if (oldMouseY == y)
		return;

	/* Compute scrollbar ymin and ymax values */ 

	scrollYmin = (fsdlg[SGFSDLG_SCROLLBAR].y + fsdlg[0].y) * sdlgui_fontheight;
	scrollYmax = (fsdlg[SGFSDLG_DOWN].y + fsdlg[0].y) * sdlgui_fontheight;
	
	scrollY = fsdlg[SGFSDLG_SCROLLBAR].y * sdlgui_fontheight + fsdlg[SGFSDLG_SCROLLBAR].h + fsdlg[0].y * sdlgui_fontheight;
	scrollH_half = scrollY + fsdlg[SGFSDLG_SCROLLBAR].w / 2;
	scrollMove = (float)(y-oldMouseY)/sdlgui_fontheight;
	
	/* Verify if mouse is not above the scrollbar area */
	if (y < scrollYmin) {
		mouseIsOut = SCROLLOUT_ABOVE;
		oldMouseY = y;
		return;
	}
	if (mouseIsOut == SCROLLOUT_ABOVE && y < scrollH_half) {
		oldMouseY = y;
		return;
	}

	/* Verify if mouse is not under the scrollbar area */
	if (y > scrollYmax) {
		mouseIsOut = SCROLLOUT_UNDER;
		oldMouseY = y;
		return;
	}
	if (mouseIsOut == SCROLLOUT_UNDER && y > scrollH_half) {
		oldMouseY = y;
		return;
	}

	mouseIsOut = 0;

	scrollbar_Ypos += scrollMove;
	oldMouseY = y;

	/* Verifiy if scrollbar is in correct inferior boundary */
	if (scrollbar_Ypos < 0)
		scrollbar_Ypos = 0.0;

	/* Verifiy if scrollbar is in correct superior boundary */
	b = (int) (scrollbar_Ypos * ((float)entries/(float)(SGFS_NUMENTRIES-2)) + 0.5);
	if (b+SGFS_NUMENTRIES >= entries) {
		ypos = entries - SGFS_NUMENTRIES;
		DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
	}

	refreshentries = true;			
}

/*-----------------------------------------------------------------------*/
/**
 * Handle SDL events.
 */
static void DlgFileSelect_HandleSdlEvents(int/*SDL_Event*/ *pEvent)
{
	int oldypos = ypos;

//RETRO TODO FIX
#if 0
	switch (pEvent->type)
	{
	 case SDL_MOUSEBUTTONDOWN:
		if (pEvent->button.button == SDL_BUTTON_WHEELUP)
			DlgFileSelect_ScrollUp();
		else if (pEvent->button.button == SDL_BUTTON_WHEELDOWN)
			DlgFileSelect_ScrollDown();
		break;
	 case SDL_KEYDOWN:		
		switch (pEvent->key.keysym.sym)
		{
		 case SDLK_UP:
			DlgFileSelect_ScrollUp();
			break;
		 case SDLK_DOWN:
			DlgFileSelect_ScrollDown();
			break;
		 case SDLK_HOME:
			ypos = 0; 
			DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
			break;
		 case SDLK_END:
		    ypos = entries-SGFS_NUMENTRIES; 
			DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
		    break;
		 case SDLK_PAGEUP:
		    ypos -= SGFS_NUMENTRIES;
			DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
		    break;
		 case SDLK_PAGEDOWN:
			if (ypos+2*SGFS_NUMENTRIES < entries)
				ypos += SGFS_NUMENTRIES;
			else
				ypos = entries-SGFS_NUMENTRIES;
			DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
			break;
		 default:
			break;
		}
		break;
	default:
		break;
	}
#endif
	if (ypos < 0) {
		ypos = 0;
		scrollbar_Ypos = 0.0;
	}
	
	if (ypos != oldypos)
		refreshentries = true;
}


/*-----------------------------------------------------------------------*/
/**
 * Free file entries
 */
static struct dirent **files_free(struct dirent **files)
{
	int i;
	if (files != NULL)
	{
		for(i=0; i<entries; i++)
		{
			free(files[i]);
		}
		free(files);
	}
	return NULL;
}


/*-----------------------------------------------------------------------*/
/**
 * Copy to dst src+add if they are below maxlen and return true,
 * otherwise return false
 */
static int strcat_maxlen(char *dst, int maxlen, const char *src, const char *add)
{
	int slen, alen;
	slen = strlen(src);
	alen = strlen(add);
	if (slen + alen < maxlen)
	{
		strcpy(dst, src);
		strcpy(dst+slen, add);
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------*/
/**
 * Create and return suitable path into zip file
 */
static char* zip_get_path(const char *zipdir, const char *zipfilename, int browsingzip)
{
	if (browsingzip)
	{
		char *zippath;
		zippath = malloc(strlen(zipdir) + strlen(zipfilename) + 1);
		strcpy(zippath, zipdir);
		strcat(zippath, zipfilename);
		return zippath;
	}
	return strdup("");
}

/**
 * string for zip root needs to be empty, check and correct if needed
 */
static void correct_zip_root(char *zippath)
{
	if (zippath[0] == PATHSEP && !zippath[1])
	{
		zippath[0] = '\0';
	}
}

/**
 * Convert Ypos to Y scrollbar position
 */
static void DlgFileSelect_Convert_ypos_to_scrollbar_Ypos(void)
{
	if (entries <= SGFS_NUMENTRIES)
		scrollbar_Ypos = 0.0;
	else
		scrollbar_Ypos = (float)ypos / ((float)entries/(float)(SGFS_NUMENTRIES-2));
}

//RETRO 
extern int okold;

/*-----------------------------------------------------------------------*/
/**
 * Show and process a file selection dialog.
 * Returns path/name user selected or NULL if user canceled
 * input: zip_path = pointer's pointer to buffer to contain file path
 * within a selected zip file, or NULL if browsing zip files is disallowed.
 * bAllowNew: true if the user is allowed to insert new file names.
 */
char* SDLGui_FileSelect(const char *path_and_name, char **zip_path, bool bAllowNew)
{
	struct dirent **files = NULL;
	char *pStringMem;
	char *retpath = NULL;
	const char *home;
	char *path, *fname;                 /* The actual file and path names */
	bool reloaddir = true;              /* Do we have to reload the directory file list? */
	int retbut;
	//bool bOldMouseVisibility;
	int selection;                      /* The selection index */
	char *zipfilename;                  /* Filename in zip file */
	char *zipdir;
	bool browsingzip = false;           /* Are we browsing an archive? */
	zip_dir *zipfiles = NULL;
	int /*SDL_Event*/ sdlEvent;
	int yScrollbar_size;                 /* Size of the vertical scrollbar */

	/* If this is the first call to SDLGui_FileSelect, we reset scrollbar_Ypos and ypos */
	/* Else, we keep the previous value of scrollbar_Ypos and update ypos below, to open */
	/* the fileselector at the same position it was used */
	if ( ypos < 0 )
	{
		scrollbar_Ypos = 0.0;
		ypos = 0;
	}

	refreshentries = true;
	entries = 0;

	/* Allocate memory for the file and path name strings: */
	pStringMem = malloc(4 * FILENAME_MAX);
	path = pStringMem;
	fname = pStringMem + FILENAME_MAX;
	zipdir = pStringMem + 2 * FILENAME_MAX;
	zipfilename = pStringMem + 3 * FILENAME_MAX;
	zipfilename[0] = 0;
	fname[0] = 0;
	path[0] = 0;
#if 0
	/* Save mouse state and enable cursor */
	bOldMouseVisibility = SDL_ShowCursor(SDL_QUERY);
	SDL_ShowCursor(SDL_ENABLE);
#endif
	SDLGui_CenterDlg(fsdlg);
	if (bAllowNew)
	{
		fsdlg[SGFSDLG_FILENAME].type = SGEDITFIELD;
		fsdlg[SGFSDLG_FILENAME].flags |= SG_EXIT;
	}
	else
	{
		fsdlg[SGFSDLG_FILENAME].type = SGTEXT;
		fsdlg[SGFSDLG_FILENAME].flags &= ~SG_EXIT;
	}

	/* Prepare the path and filename variables */
	if (path_and_name && path_and_name[0])
	{
		strncpy(path, path_and_name, FILENAME_MAX);
		path[FILENAME_MAX-1] = '\0';
	}
	if (!File_DirExists(path))
	{
		File_SplitPath(path, path, fname, NULL);
		if (!(File_DirExists(path) || getcwd(path, FILENAME_MAX)))
		{
			perror("SDLGui_FileSelect: non-existing path and CWD failed");
			goto clean_exit;
		}
	}

	File_MakeAbsoluteName(path);
	File_MakeValidPathName(path);
	File_ShrinkName(dlgpath, path, DLGPATH_SIZE);
	File_ShrinkName(dlgfname, fname, DLGFNAME_SIZE);

	okold=0;

	do
	{
		if (reloaddir)
		{
			files = files_free(files);

			if (browsingzip)
			{
				files = ZIP_GetFilesDir(zipfiles, zipdir, &entries);
				if(!files)
				{
					fprintf(stderr, "SDLGui_FileSelect: ZIP_GetFilesDir error!\n");
					goto clean_exit;
				}
			}
			else
			{
				/* Load directory entries: */
				entries = scandir(path, &files, 0, alphasort2);
			}
			
			/* Remove hidden files from the list if necessary: */
			if (!(fsdlg[SGFSDLG_SHOWHIDDEN].state & SG_SELECTED))
			{
				DlgFileSelect_RemoveHiddenFiles(files);
			}

			if (entries < 0)
			{
				fprintf(stderr, "SDLGui_FileSelect: Path not found.\n");
				goto clean_exit;
			}

			/* reload always implies refresh */
			reloaddir = false;
			refreshentries = true;
		}/* reloaddir */

		/* Refresh scrollbar size */
 		if (entries <= SGFS_NUMENTRIES)
			yScrollbar_size = (SGFS_NUMENTRIES-2) * sdlgui_fontheight;
		else
		{
			yScrollbar_size = (int)((SGFS_NUMENTRIES-2) / ((float)entries/(float)SGFS_NUMENTRIES) * sdlgui_fontheight);
			if ( yScrollbar_size < SCROLLBAR_MIN_HEIGHT )		/* Value could be 0 for very large directory */
				yScrollbar_size = SCROLLBAR_MIN_HEIGHT;
		}
		fsdlg[SGFSDLG_SCROLLBAR].w = yScrollbar_size;

		/* Refresh scrolbar pos */
		ypos = (int) (scrollbar_Ypos * ((float)entries/(float)(SGFS_NUMENTRIES-2)) + 0.5);

		if (ypos+SGFS_NUMENTRIES >= entries) {			/* Ensure Y pos is in the correct boundaries */
			ypos = entries - SGFS_NUMENTRIES;
			if ( ypos < 0 )
				ypos = 0;
			DlgFileSelect_Convert_ypos_to_scrollbar_Ypos();
		}
		fsdlg[SGFSDLG_SCROLLBAR].h = (int) (scrollbar_Ypos * sdlgui_fontheight);

		/* Update the file name strings in the dialog? */
		if (refreshentries)
		{
			if (!DlgFileSelect_RefreshEntries(files, path, browsingzip))
			{
				goto clean_exit;
			}
			refreshentries = false;
		}

		/* Show dialog: */
		retbut = SDLGui_DoDialog(fsdlg, &sdlEvent);

		/* Has the user clicked on a file or folder? */
		if (retbut>=SGFSDLG_ENTRYFIRST && retbut<=SGFSDLG_ENTRYLAST && retbut-SGFSDLG_ENTRYFIRST+ypos<entries)
		{
			char *tempstr;
			
			tempstr = malloc(FILENAME_MAX);
			if (!tempstr)
			{
				perror("Error while allocating temporary memory in SDLGui_FileSelect()");
				goto clean_exit;
			}

			if (browsingzip == true)
			{
				if (!strcat_maxlen(tempstr, FILENAME_MAX,
						   zipdir, files[retbut-SGFSDLG_ENTRYFIRST+ypos]->d_name))
				{
					fprintf(stderr, "SDLGui_FileSelect: Path name too long!\n");
					goto clean_exit;
				}
				/* directory? */
				if (File_DoesFileNameEndWithSlash(tempstr))
				{
					/* handle the ../ directory */
					if (strcmp(files[retbut-SGFSDLG_ENTRYFIRST+ypos]->d_name, "../") == 0)
					{
						/* close the zip file */
						if (strcmp(tempstr, "../") == 0)
						{
							/* free zip file entries */
							ZIP_FreeZipDir(zipfiles);
							zipfiles = NULL;
							/* Copy the path name to the dialog */
							File_ShrinkName(dlgpath, path, DLGPATH_SIZE);
							browsingzip = false;
						}
						else
						{
							/* remove "../" and previous dir from path */
							File_PathShorten(tempstr, 2);
							correct_zip_root(tempstr);
							strcpy(zipdir, tempstr);
							File_ShrinkName(dlgpath, zipdir, DLGPATH_SIZE);
						}
					}
					else /* not the "../" directory */
					{
						strcpy(zipdir, tempstr);
						File_ShrinkName(dlgpath, zipdir, DLGPATH_SIZE);
					}
					reloaddir = true;
					/* Copy the path name to the dialog */
					zipfilename[0] = '\0';
					dlgfname[0] = 0;
					ypos = 0;
					scrollbar_Ypos = 0.0;
				}
				else
				{
					/* not dir, select a file in the zip */
					selection = retbut-SGFSDLG_ENTRYFIRST+ypos;
					strcpy(zipfilename, files[selection]->d_name);
					File_ShrinkName(dlgfname, zipfilename, DLGFNAME_SIZE);
				}

			}
			else /* not browsingzip */
			{
				if (!strcat_maxlen(tempstr, FILENAME_MAX,
						   path, files[retbut-SGFSDLG_ENTRYFIRST+ypos]->d_name))
				{
					fprintf(stderr, "SDLGui_FileSelect: Path name too long!\n");
					goto clean_exit;
				}
				if (File_DirExists(tempstr))
				{
					File_HandleDotDirs(tempstr);
					File_AddSlashToEndFileName(tempstr);
					/* Copy the path name to the dialog */
					File_ShrinkName(dlgpath, tempstr, DLGPATH_SIZE);
					strcpy(path, tempstr);
					reloaddir = true;
					dlgfname[0] = 0;
					ypos = 0;
					scrollbar_Ypos = 0.0;
				}
				else if (ZIP_FileNameIsZIP(tempstr) && zip_path != NULL)
				{
					/* open a zip file */
					zipfiles = ZIP_GetFiles(tempstr);
					if (zipfiles != NULL && browsingzip == false)
					{
						selection = retbut-SGFSDLG_ENTRYFIRST+ypos;
						strcpy(fname, files[selection]->d_name);
						File_ShrinkName(dlgfname, fname, DLGFNAME_SIZE);
						browsingzip = true;
						zipdir[0] = '\0'; /* zip root */
						File_ShrinkName(dlgpath, zipdir, DLGPATH_SIZE);
						reloaddir = true;
						ypos = 0;
						scrollbar_Ypos = 0.0;
					}

				}
				else
				{
					/* Select a file */
					selection = retbut-SGFSDLG_ENTRYFIRST+ypos;
					strcpy(fname, files[selection]->d_name);
					File_ShrinkName(dlgfname, fname, DLGFNAME_SIZE);
				}

			} /* not browsingzip */

			free(tempstr);
		}
		else    /* Has the user clicked on another button? */
		{
			switch(retbut)
			{
			case SGFSDLG_UPDIR:                 /* Change path to parent directory */
				if (browsingzip)
				{
					/* close the zip file? */
					if (!zipdir[0])
					{
						/* free zip file entries */
						ZIP_FreeZipDir(zipfiles);
						browsingzip = false;
						zipfiles = NULL;
						File_ShrinkName(dlgpath, path, DLGPATH_SIZE);
					}
					else
					{
						/* remove last dir from zipdir path */
						File_PathShorten(zipdir, 1);
						correct_zip_root(zipdir);
						File_ShrinkName(dlgpath, zipdir, DLGPATH_SIZE);
						zipfilename[0] = '\0';
					}
				}  /* not a zip file: */
				else
				{
					File_PathShorten(path, 1);
					File_ShrinkName(dlgpath, path, DLGPATH_SIZE);
				}
				reloaddir = true;
				break;

			case SGFSDLG_HOMEDIR:               /* Change to home directory */
			case SGFSDLG_CWD:                   /* Change to current work directory */
				if (retbut == SGFSDLG_CWD)
					home = Paths_GetWorkingDir();
				else
					home = Paths_GetUserHome();
				if (home == NULL || !*home)
					break;
				if (browsingzip)
				{
					/* free zip file entries */
					ZIP_FreeZipDir(zipfiles);
					zipfiles = NULL;
					browsingzip = false;
				}
				strcpy(path, home);
				File_AddSlashToEndFileName(path);
				File_ShrinkName(dlgpath, path, DLGPATH_SIZE);
				reloaddir = true;
				break;

			case SGFSDLG_ROOTDIR:               /* Change to root directory */
				if (browsingzip)
				{
					/* free zip file entries */
					ZIP_FreeZipDir(zipfiles);
					zipfiles = NULL;
					browsingzip = false;
				}
				path[0] = PATHSEP; path[1] = '\0';
				strcpy(dlgpath, path);
				reloaddir = true;
				break;
			case SGFSDLG_UP:                    /* Scroll up */
				DlgFileSelect_ScrollUp();
				usleep(10*1000); /*us*/
				break;
			case SGFSDLG_DOWN:                  /* Scroll down */
				DlgFileSelect_ScrollDown();
				usleep(10*1000); /*us*/
				break;
			case SGFSDLG_SCROLLBAR:             /* Scrollbar selected */
				DlgFileSelect_ManageScrollbar();
				usleep(10*1000); /*us*/
				break;
			case SGFSDLG_FILENAME:              /* User entered new filename */
				strcpy(fname, dlgfname);
				break;
			case SGFSDLG_SHOWHIDDEN:            /* Show/hide hidden files */
				reloaddir = true;
				ypos = 0;
				scrollbar_Ypos = 0.0;
				break;
			case SDLGUI_UNKNOWNEVENT:
				DlgFileSelect_HandleSdlEvents(&sdlEvent);
				break;
			} /* switch */
      
			if (reloaddir)
			{
				/* Remove old selection */
				fname[0] = 0;
				dlgfname[0] = 0;
				ypos = 0;
				scrollbar_Ypos = 0.0;
			}
		} /* other button code */
                gui_poll_events();

	} /* do */
	while (retbut!=SGFSDLG_OKAY && retbut!=SGFSDLG_CANCEL
	       && retbut!=SDLGUI_QUIT && retbut != SDLGUI_ERROR && !bQuitProgram);

	files_free(files);

	if (browsingzip)
	{
		/* free zip file entries */
		ZIP_FreeZipDir(zipfiles);
		zipfiles = NULL;
	}

	if (retbut == SGFSDLG_OKAY)
	{
		if (zip_path)
			*zip_path = zip_get_path(zipdir, zipfilename, browsingzip);
		retpath = File_MakePath(path, fname, NULL);
	}
	else
		retpath = NULL;

clean_exit:
//	SDL_ShowCursor(bOldMouseVisibility);
	free(pStringMem);

	return retpath;
}


/*-----------------------------------------------------------------------*/
/**
 * Let user browse for a file, confname is used as default.
 * If bAllowNew is true, user can select new files also.
 * 
 * If no file is selected, or there's some problem with the file,
 * return false and clear dlgname & confname.
 * Otherwise return true, set dlgname & confname to the new file name
 * (dlgname is shrunken & limited to maxlen and confname is assumed
 * to have FILENAME_MAX amount of space).
 */
bool SDLGui_FileConfSelect(char *dlgname, char *confname, int maxlen, bool bAllowNew)
{
	char *selname;
	
	selname = SDLGui_FileSelect(confname, NULL, bAllowNew);

	if (selname)
	{
		if (!File_DoesFileNameEndWithSlash(selname) &&
		    (bAllowNew || File_Exists(selname)))
		{
			strncpy(confname, selname, FILENAME_MAX);
			confname[FILENAME_MAX-1] = '\0';
			File_ShrinkName(dlgname, selname, maxlen);

		}
		else
		{
			dlgname[0] = confname[0] = 0;
		}
		free(selname);

		return true;
	}
	return false;
}
