/*
	modded for libretro-uae
*/

/*
  Hatari

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_PATHS_H
#define HATARI_PATHS_H

#include <stdio.h>

#ifdef WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

#define BIN2DATADIR "."

extern void Paths_Init(const char *argv0);
extern const char *Paths_GetWorkingDir(void);
extern const char *Paths_GetDataDir(void);
extern const char *Paths_GetUserHome(void);
extern const char *Paths_GetHatariHome(void);

#endif
