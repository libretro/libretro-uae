/*
	modded for libretro-uae
*/

/*
  Hatari - file.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Common file access functions.
*/
const char File_fileid[] = "Hatari file.c : " __DATE__ " " __TIME__;

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <zlib.h>

#include "utype.h"

#if defined(WIN32) && !defined(_VCWIN_)
#include <winsock2.h>
#endif

#include "dialog.h"
#include "file.h"
#include "str.h"
#include "zip.h"

#if defined(WIN32)
#define ftello ftell
#endif

/*-----------------------------------------------------------------------*/
/**
 * Remove any '/'s from end of filenames, but keeps / intact
 */
void File_CleanFileName(char *pszFileName)
{
	int len;

	len = strlen(pszFileName);

	/* Remove end slashes from filename! But / remains! Doh! */
	while (len > 2 && pszFileName[--len] == PATHSEP)
		pszFileName[len] = '\0';
}


/*-----------------------------------------------------------------------*/
/**
 * Add '/' to end of filename
 */
void File_AddSlashToEndFileName(char *pszFileName)
{
	int len;

	len = strlen(pszFileName);

	/* Check dir/filenames */
	if (len != 0)
	{
		if (pszFileName[len-1] != PATHSEP)
		{
			pszFileName[len] = PATHSEP; /* Must use end slash */
			pszFileName[len+1] = '\0';
		}
	}
}


/*-----------------------------------------------------------------------*/
/**
 * Does filename extension match? If so, return TRUE
 */
bool File_DoesFileExtensionMatch(const char *pszFileName, const char *pszExtension)
{
	if (strlen(pszFileName) < strlen(pszExtension))
		return false;
	/* Is matching extension? */
	if (!strcasecmp(&pszFileName[strlen(pszFileName)-strlen(pszExtension)], pszExtension))
		return true;

	/* No */
	return false;
}


/*-----------------------------------------------------------------------*/
/**
 * Check if filename is from root
 *
 * Return TRUE if filename is '/', else give FALSE
 */
static bool File_IsRootFileName(const char *pszFileName)
{
	if (pszFileName[0] == '\0')     /* If NULL string return! */
		return false;

	if (pszFileName[0] == PATHSEP)
		return true;

#ifdef WIN32
	if (pszFileName[1] == ':')
		return true;
#endif

#ifdef GEKKO
	if (strlen(pszFileName) > 2 && pszFileName[2] == ':')	// sd:
		return true;
	if (strlen(pszFileName) > 3 && pszFileName[3] == ':')	// fat:
		return true;
	if (strlen(pszFileName) > 4 && pszFileName[4] == ':')	// fat3:
		return true;
#endif

	return false;
}


/*-----------------------------------------------------------------------*/
/**
 * Return string, to remove 'C:' part of filename
 */
const char *File_RemoveFileNameDrive(const char *pszFileName)
{
	if ( (pszFileName[0] != '\0') && (pszFileName[1] == ':') )
		return &pszFileName[2];
	else
		return pszFileName;
}


/*-----------------------------------------------------------------------*/
/**
 * Check if filename end with a '/'
 *
 * Return TRUE if filename ends with '/'
 */
bool File_DoesFileNameEndWithSlash(char *pszFileName)
{
	if (pszFileName[0] == '\0')    /* If NULL string return! */
		return false;

	/* Does string end in a '/'? */
	if (pszFileName[strlen(pszFileName)-1] == PATHSEP)
		return true;

	return false;
}


/*-----------------------------------------------------------------------*/
/**
 * Read file from disk into allocated buffer and return the buffer
 * or NULL for error.  If pFileSize is non-NULL, read file size
 * is set to that.
 */
Uint8 *File_Read(const char *pszFileName, long *pFileSize, const char * const ppszExts[])
{
	char *filepath = NULL;
	Uint8 *pFile = NULL;
	long FileSize = 0;

	/* Does the file exist? If not, see if can scan for other extensions and try these */
	if (!File_Exists(pszFileName) && ppszExts)
	{
		/* Try other extensions, if succeeds, returns correct one */
		filepath = File_FindPossibleExtFileName(pszFileName, ppszExts);
	}
	if (!filepath)
		filepath = strdup(pszFileName);

	/* Is it a gzipped file? */
	if (File_DoesFileExtensionMatch(filepath, ".gz"))
	{
		gzFile hGzFile;
		/* Open and read gzipped file */
		hGzFile = gzopen(filepath, "rb");
		if (hGzFile != NULL)
		{
			/* Find size of file: */
			do
			{
				/* Seek through the file until we hit the end... */
				char tmp[1024];
				if (gzread(hGzFile, tmp, sizeof(tmp)) < 0)
				{
					fprintf(stderr, "Failed to read gzip file!\n");
					return NULL;
				}
			}
			while (!gzeof(hGzFile));
			FileSize = gztell(hGzFile);
			gzrewind(hGzFile);
			/* Read in... */
			pFile = malloc(FileSize);
			if (pFile)
				FileSize = gzread(hGzFile, pFile, FileSize);

			gzclose(hGzFile);
		}
	}
	else if (File_DoesFileExtensionMatch(filepath, ".zip"))
	{
		/* It is a .ZIP file! -> Try to load the first file in the archive */
		pFile = ZIP_ReadFirstFile(filepath, &FileSize, ppszExts);
	}
	else          /* It is a normal file */
	{
		FILE *hDiskFile;
		/* Open and read normal file */
		hDiskFile = fopen(filepath, "rb");
		if (hDiskFile != NULL)
		{
			/* Find size of file: */
			fseek(hDiskFile, 0, SEEK_END);
			FileSize = ftell(hDiskFile);
			fseek(hDiskFile, 0, SEEK_SET);
			/* Read in... */
			pFile = malloc(FileSize);
			if (pFile)
				FileSize = fread(pFile, 1, FileSize, hDiskFile);

			fclose(hDiskFile);
		}
	}
	free(filepath);

	/* Store size of file we read in (or 0 if failed) */
	if (pFileSize)
		*pFileSize = FileSize;

	return pFile;        /* Return to where read in/allocated */
}


/*-----------------------------------------------------------------------*/
/**
 * Save file to disk, return FALSE if errors
 */
bool File_Save(const char *pszFileName, const Uint8 *pAddress, size_t Size, bool bQueryOverwrite)
{
	bool bRet = false;

	/* Check if need to ask user if to overwrite */
	if (bQueryOverwrite)
	{
		/* If file exists, ask if OK to overwrite */
		if (!File_QueryOverwrite(pszFileName))
			return false;
	}

	/* Normal file or gzipped file? */
	if (File_DoesFileExtensionMatch(pszFileName, ".gz"))
	{
		gzFile hGzFile;
		/* Create a gzipped file: */
		hGzFile = gzopen(pszFileName, "wb");
		if (hGzFile != NULL)
		{
			/* Write data, set success flag */
			if (gzwrite(hGzFile, pAddress, Size) == (int)Size)
				bRet = true;

			gzclose(hGzFile);
		}
	}
	else
	{
		FILE *hDiskFile;
		/* Create a normal file: */
		hDiskFile = fopen(pszFileName, "wb");
		if (hDiskFile != NULL)
		{
			/* Write data, set success flag */
			if (fwrite(pAddress, 1, Size, hDiskFile) == Size)
				bRet = true;

			fclose(hDiskFile);
		}
	}

	return bRet;
}


/*-----------------------------------------------------------------------*/
/**
 * Return size of file, -1 if error
 */
off_t File_Length(const char *pszFileName)
{
	FILE *hDiskFile;
	off_t FileSize;

	hDiskFile = fopen(pszFileName, "rb");
	if (hDiskFile!=NULL)
	{
		fseek(hDiskFile, 0, SEEK_END);
		FileSize = ftello(hDiskFile);
		fseek(hDiskFile, 0, SEEK_SET);
		fclose(hDiskFile);
		return FileSize;
	}

	return -1;
}


/*-----------------------------------------------------------------------*/
/**
 * Return TRUE if file exists, is readable or writable at least and is not
 * a directory.
 */
bool File_Exists(const char *filename)
{
//RETRO HACK
#ifdef RETRO

if( access( filename, F_OK ) != -1 ) {
    // file exists
	return true;
} else {
    // file doesn't exist
	return false;
}
#else
	struct stat buf;
	if (stat(filename, &buf) == 0 &&
	    (buf.st_mode & (S_IRUSR|S_IWUSR)) && !(buf.st_mode & S_IFDIR))
	{
		/* file points to user readable regular file */
		return true;
	}
	return false;
#endif
}


/*-----------------------------------------------------------------------*/
/**
 * Return TRUE if directory exists.
 */
#if 0
bool File_DirExists(const char *path)
{
	struct stat buf;
	return (stat(path, &buf) == 0 && S_ISDIR(buf.st_mode));
}
#endif

/*-----------------------------------------------------------------------*/
/**
 * Find if file exists, and if so ask user if OK to overwrite
 */
bool File_QueryOverwrite(const char *pszFileName)
{
	const char *fmt;
	char *szString;
	bool ret = true;

	/* Try and find if file exists */
	if (File_Exists(pszFileName))
	{
		fmt = "File '%s' exists, overwrite?";
		/* File does exist, are we OK to overwrite? */
		szString = malloc(strlen(pszFileName) + strlen(fmt) + 1);
		sprintf(szString, fmt, pszFileName);
		fprintf(stderr, "%s\n", szString);
		ret = DlgAlert_Query(szString);
		free(szString);
	}
	return ret;
}


/*-----------------------------------------------------------------------*/
/**
 * Try filename with various extensions and check if file exists
 * - if so, return allocated string which caller should free,
 *   otherwise return NULL
 */
char * File_FindPossibleExtFileName(const char *pszFileName, const char * const ppszExts[])
{
	char *szSrcDir, *szSrcName, *szSrcExt;
	int i;

	/* Allocate temporary memory for strings: */
	szSrcDir = malloc(3 * FILENAME_MAX);
	if (!szSrcDir)
	{
		perror("File_FindPossibleExtFileName");
		return false;
	}
	szSrcName = szSrcDir + FILENAME_MAX;
	szSrcExt = szSrcName + FILENAME_MAX;

	/* Split filename into parts */
	File_SplitPath(pszFileName, szSrcDir, szSrcName, szSrcExt);

	/* Scan possible extensions */
	for (i = 0; ppszExts[i]; i++)
	{
		char *szTempFileName;

		/* Re-build with new file extension */
		szTempFileName = File_MakePath(szSrcDir, szSrcName, ppszExts[i]);
		if (szTempFileName)
		{
			/* Does this file exist? */
			if (File_Exists(szTempFileName))
			{
				free(szSrcDir);
				/* return filename without extra strings */
				return szTempFileName;
			}
			free(szTempFileName);
		}
	}
	free(szSrcDir);
	return NULL;
}


/*-----------------------------------------------------------------------*/
/**
 * Split a complete filename into path, filename and extension.
 * If pExt is NULL, don't split the extension from the file name!
 * It's safe for pSrcFileName and pDir to be the same string.
 */
void File_SplitPath(const char *pSrcFileName, char *pDir, char *pName, char *pExt)
{
	char *ptr1, *ptr2;

	/* Build pathname: */
	ptr1 = strrchr(pSrcFileName, PATHSEP);
	if (ptr1)
	{
		strcpy(pName, ptr1+1);
		memmove(pDir, pSrcFileName, ptr1-pSrcFileName);
		pDir[ptr1-pSrcFileName] = 0;
	}
	else
	{
 		strcpy(pName, pSrcFileName);
		sprintf(pDir, ".%c", PATHSEP);
	}

	/* Build the raw filename: */
	if (pExt != NULL)
	{
		ptr2 = strrchr(pName+1, '.');
		if (ptr2)
		{
			pName[ptr2-pName] = 0;
			/* Copy the file extension: */
			strcpy(pExt, ptr2+1);
		}
		else
			pExt[0] = 0;
	}
}


/*-----------------------------------------------------------------------*/
/**
 * Construct a complete filename from path, filename and extension.
 * Return the constructed filename.
 * pExt can also be NULL.
 */
char * File_MakePath(const char *pDir, const char *pName, const char *pExt)
{
	char *filepath;
	int len;

	/* dir or "." + "/" + name + "." + ext + \0 */
	len = strlen(pDir) + 2 + strlen(pName) + 1 + (pExt ? strlen(pExt) : 0) + 1;
	filepath = malloc(len);
	if (!filepath)
	{
		perror("File_MakePath");
		return NULL;
	}
	if (!pDir[0])
	{
		filepath[0] = '.';
		filepath[1] = '\0';
	} else {
		strcpy(filepath, pDir);
	}
	len = strlen(filepath);
	if (filepath[len-1] != PATHSEP)
	{
		filepath[len++] = PATHSEP;
	}
	strcpy(&filepath[len], pName);

	if (pExt != NULL && pExt[0])
	{
		len += strlen(pName);
		if (pExt[0] != '.')
			strcat(&filepath[len++], ".");
		strcat(&filepath[len], pExt);
	}
	return filepath;
}


/*-----------------------------------------------------------------------*/
/**
 * Shrink a file name to a certain length and insert some dots if we cut
 * something away (useful for showing file names in a dialog).
 */
void File_ShrinkName(char *pDestFileName, const char *pSrcFileName, int maxlen)
{
	int srclen = strlen(pSrcFileName);
	if (srclen < maxlen)
		strcpy(pDestFileName, pSrcFileName);  /* It fits! */
	else
	{
		assert(maxlen > 6);
		strncpy(pDestFileName, pSrcFileName, maxlen/2);
		if (maxlen&1)  /* even or uneven? */
			pDestFileName[maxlen/2-1] = 0;
		else
			pDestFileName[maxlen/2-2] = 0;
		strcat(pDestFileName, "...");
		strcat(pDestFileName, &pSrcFileName[strlen(pSrcFileName)-maxlen/2+1]);
	}
}


/*-----------------------------------------------------------------------*/
/**
 * Open given filename in given mode and handle "stdout" & "stderr"
 * filenames specially. Return FILE* to the opened file or NULL on error.
 */
FILE *File_Open(const char *path, const char *mode)
{
	int wr = 0, rd = 0;
	FILE *fp;

	/* empty name signifies file that shouldn't be opened/enabled */
	if (!*path)
		return NULL;

	/* special "stdout" and "stderr" files can be used
	 * for files which are written or appended
	 */
	if (strchr(mode, 'w') || strchr(mode, 'a'))
		wr = 1;
	if (strchr(mode, 'r'))
		rd = 1;
	if (strcmp(path, "stdin") == 0)
	{
		assert(rd && !wr);
		return stdin;
	}
	if (strcmp(path, "stdout") == 0)
	{
		assert(wr && !rd);
		return stdout;
	}
	if (strcmp(path, "stderr") == 0)
	{
		assert(wr && !rd);
		return stderr;
	}
	/* Open a normal log file */
	fp = fopen(path, mode);
	if (!fp)
		fprintf(stderr, "Can't open file '%s' (wr=%i, rd=%i):\n  %s\n",
			path, wr, rd, strerror(errno));

	/* printf("'%s' opened in mode '%s'\n", path, mode, fp); */
	return fp;
}


/*-----------------------------------------------------------------------*/
/**
 * Close given FILE pointer and return the closed pointer
 * as NULL for the idiom "fp = File_Close(fp);"
 */
FILE *File_Close(FILE *fp)
{
	if (fp && fp != stdin && fp != stdout && fp != stderr)
	{
		fclose(fp);
	}
	return NULL;
}


/*-----------------------------------------------------------------------*/
/**
 * Check if input is available at the specified file descriptor.
 */
bool File_InputAvailable(FILE *fp)
{
#if HAVE_SELECT
	fd_set rfds;
	struct timeval tv;
	int fh;
	int ret;

	if (!fp || (fh = fileno(fp)) == -1)
		return false;

	/* Add the file handle to the file descriptor set */
	FD_ZERO(&rfds);
	FD_SET(fh, &rfds);

	/* Return immediately */
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	/* Check if file descriptor is ready for a read */
	ret = select(fh+1, &rfds, NULL, NULL, &tv);

	if (ret > 0)
		return true;    /* Data available */
#endif

	return false;
}


/*-----------------------------------------------------------------------*/
/**
 * Wrapper for File_MakeAbsoluteName() which special-cases stdin/out/err
 * named files and empty file name.  The given buffer should be opened
 * with File_Open() and closed with File_Close() if this function is used!
 * (On Linux one can use /dev/stdout etc, this is intended for other OSes)
 */
void File_MakeAbsoluteSpecialName(char *path)
{
	if (path[0] &&
	    strcmp(path, "stdin")  != 0 &&
	    strcmp(path, "stdout") != 0 &&
	    strcmp(path, "stderr") != 0)
		File_MakeAbsoluteName(path);
}

/*-----------------------------------------------------------------------*/
/**
 * Create a clean absolute file name from a (possibly) relative file name.
 * I.e. filter out all occurancies of "./" and "../".
 * pFileName needs to point to a buffer of at least FILENAME_MAX bytes.
 */
void File_MakeAbsoluteName(char *pFileName)
{
	char *pTempName;
	int inpos, outpos;

#if defined (__AMIGAOS4__)
	/* This function does not work on Amiga OS */
	return;
#endif

	inpos = 0;
	pTempName = malloc(FILENAME_MAX);
	if (!pTempName)
	{
		perror("File_MakeAbsoluteName - malloc");
		return;
	}

	/* Is it already an absolute name? */
	if (File_IsRootFileName(pFileName))
	{
		outpos = 0;
	}
	else
	{
		if (!getcwd(pTempName, FILENAME_MAX))
		{
			perror("File_MakeAbsoluteName - getcwd");
			free(pTempName);
			return;
		}
		File_AddSlashToEndFileName(pTempName);
		outpos = strlen(pTempName);
	}

	/* Now filter out the relative paths "./" and "../" */
	while (pFileName[inpos] != 0 && outpos < FILENAME_MAX)
	{
		if (pFileName[inpos] == '.' && pFileName[inpos+1] == PATHSEP)
		{
			/* Ignore "./" */
			inpos += 2;
		}
		else if (pFileName[inpos] == '.' && pFileName[inpos+1] == 0)
		{
			inpos += 1;        /* Ignore "." at the end of the path string */
			if (outpos > 1)
				pTempName[outpos - 1] = 0;   /* Remove the last slash, too */
		}
		else if (pFileName[inpos] == '.' && pFileName[inpos+1] == '.'
		         && (pFileName[inpos+2] == PATHSEP || pFileName[inpos+2] == 0))
		{
			/* Handle "../" */
			char *pSlashPos;
			inpos += 2;
			pTempName[outpos - 1] = 0;
			pSlashPos = strrchr(pTempName, PATHSEP);
			if (pSlashPos)
			{
				*(pSlashPos + 1) = 0;
				outpos = strlen(pTempName);
			}
			else
			{
				pTempName[0] = PATHSEP;
				outpos = 1;
			}
			/* Were we already at the end of the string or is there more to come? */
			if (pFileName[inpos] == PATHSEP)
			{
				/* There was a slash after the '..', so skip slash and
				 * simply proceed with next part */
				inpos += 1;
			}
			else
			{
				/* We were at the end of the string, so let's remove the slash
				 * from the new string, too */
				if (outpos > 1)
					pTempName[outpos - 1] = 0;
			}
		}
		else
		{
			/* Copy until next slash or end of input string */
			while (pFileName[inpos] != 0 && outpos < FILENAME_MAX)
			{
				pTempName[outpos++] = pFileName[inpos++];
				if (pFileName[inpos - 1] == PATHSEP)
					break;
			}
		}
	}

	pTempName[outpos] = 0;

	strcpy(pFileName, pTempName);          /* Copy back */
	free(pTempName);
}


/*-----------------------------------------------------------------------*/
/**
 * Create a valid path name from a possibly invalid name by erasing invalid
 * path parts at the end of the string.  If string doesn't contain any path
 * component, it will be pointed to the root directory.  Empty string will
 * be left as-is to prevent overwriting past allocated area.
 */
void File_MakeValidPathName(char *pPathName)
{
	struct stat dirstat;
	char *pLastSlash;

	do
	{
		/* Check for a valid path */
		if (stat(pPathName, &dirstat) == 0 && S_ISDIR(dirstat.st_mode))
		{
			break;
		}

		pLastSlash = strrchr(pPathName, PATHSEP);
		if (pLastSlash)
		{
			/* Erase the (probably invalid) part after the last slash */
			*pLastSlash = 0;
		}
		else
		{
			if (pPathName[0])
			{
				/* point to root */
				pPathName[0] = PATHSEP;
				pPathName[1] = 0;
			}
			return;
		}
	}
	while (pLastSlash);

	/* Make sure that path name ends with a slash */
	File_AddSlashToEndFileName(pPathName);
}


/*-----------------------------------------------------------------------*/
/**
 * Remove given number of path elements from the end of the given path.
 * Leaves '/' at the end if path still has directories. Given path
 * may not be empty.
 */
void File_PathShorten(char *path, int dirs)
{
	int i, n = 0;
	/* ignore last char, it may or may not be '/' */
	i = strlen(path)-1;
	assert(i >= 0);
	while(i > 0 && n < dirs) {
		if (path[--i] == PATHSEP)
			n++;
	}
	if (path[i] == PATHSEP) {
		path[i+1] = '\0';
	} else {
		path[0] = PATHSEP;
		path[1] = '\0';
	}
}


/*-----------------------------------------------------------------------*/
/*
  If "/." or "/.." at end, remove that and in case of ".." remove
  also preceding dir (go one dir up).  Leave '/' at the end of
  the path.
*/
#if 0
void File_HandleDotDirs(char *path)
{
	int len = strlen(path);
	if (len >= 2 &&
	    path[len-2] == PATHSEP &&
	    path[len-1] == '.')
	{
		/* keep in same dir */
		path[len-1] = '\0';
	}
	else if (len >= 3 &&
	    path[len-3] == PATHSEP &&
	    path[len-2] == '.' &&
	    path[len-1] == '.')
	{
		/* go one dir up */
		if (len == 3) {
			path[1] = 0;		/* already root */
		} else {
			char *ptr;
			path[len-3] = 0;
			ptr = strrchr(path, PATHSEP);
			if (ptr)
				*(ptr+1) = 0;
		}
	}
}
#endif
