/*
	modded for libretro-uae
*/

/*
  Hatari - file.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_FILE_H
#define HATARI_FILE_H
#include "stdbool.h"
#include "stdio.h"
#include "utype.h"

extern void File_CleanFileName(char *pszFileName);
extern void File_AddSlashToEndFileName(char *pszFileName);
extern bool File_DoesFileExtensionMatch(const char *pszFileName, const char *pszExtension);
extern const char *File_RemoveFileNameDrive(const char *pszFileName);
extern bool File_DoesFileNameEndWithSlash(char *pszFileName);
extern Uint8 *File_Read(const char *pszFileName, long *pFileSize, const char * const ppszExts[]);
extern bool File_Save(const char *pszFileName, const Uint8 *pAddress, size_t Size, bool bQueryOverwrite);
extern off_t File_Length(const char *pszFileName);
extern bool File_Exists(const char *pszFileName);
extern bool File_DirExists(const char *psDirName);
extern bool File_QueryOverwrite(const char *pszFileName);
extern char* File_FindPossibleExtFileName(const char *pszFileName,const char * const ppszExts[]);
extern void File_SplitPath(const char *pSrcFileName, char *pDir, char *pName, char *Ext);
extern char* File_MakePath(const char *pDir, const char *pName, const char *pExt);
extern void File_ShrinkName(char *pDestFileName, const char *pSrcFileName, int maxlen);
extern FILE *File_Open(const char *path, const char *mode);
extern FILE *File_Close(FILE *fp);
extern bool File_InputAvailable(FILE *fp);
extern void File_MakeAbsoluteSpecialName(char *pszFileName);
extern void File_MakeAbsoluteName(char *pszFileName);
extern void File_MakeValidPathName(char *pPathName);
extern void File_PathShorten(char *path, int dirs);
extern void File_HandleDotDirs(char *path);

#endif /* HATARI_FILE_H */
