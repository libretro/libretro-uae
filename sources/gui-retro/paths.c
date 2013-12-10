/*
	modded for libretro-uae
*/

/*
  Hatari - paths.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Set up the various path strings.
*/
const char Paths_fileid[] = "Hatari paths.c : " __DATE__ " " __TIME__;

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "paths.h"

#if defined(WIN32) && !defined(mkdir)
#define mkdir(name,mode) mkdir(name)
#endif  /* WIN32 */

static char sWorkingDir[FILENAME_MAX];    /* Working directory */
static char sDataDir[FILENAME_MAX];       /* Directory where data files of Hatari can be found */
static char sUserHomeDir[FILENAME_MAX];   /* User's home directory ($HOME) */
static char sHatariHomeDir[FILENAME_MAX]; /* Hatari's home directory ($HOME/.hatari/) */


/**
 * Return pointer to current working directory string
 */
const char *Paths_GetWorkingDir(void)
{
	return sWorkingDir;
}

/**
 * Return pointer to data directory string
 */
const char *Paths_GetDataDir(void)
{
	return sDataDir;
}

/**
 * Return pointer to user's home directory string
 */
const char *Paths_GetUserHome(void)
{
	return sUserHomeDir;
}

/**
 * Return pointer to Hatari's home directory string
 */
const char *Paths_GetHatariHome(void)
{
	return sHatariHomeDir;
}


/**
 * Explore the PATH environment variable to see where our executable is
 * installed.
 */
static void Paths_GetExecDirFromPATH(const char *argv0, char *pExecDir, int nMaxLen)
{
	char *pPathEnv;
	char *pAct;
	char *pTmpName;
	const char *pToken;

	/* Get the PATH environment string */
	pPathEnv = getenv("PATH");
	if (!pPathEnv)
		return;
	/* Duplicate the string because strtok destroys it later */
	pPathEnv = strdup(pPathEnv);
	if (!pPathEnv)
		return;

	pTmpName = malloc(FILENAME_MAX);
	if (!pTmpName)
		return;

	/* If there is a semicolon in the PATH, we assume it is the PATH
	 * separator token (like on Windows), otherwise we use a colon. */
	if (strchr((pPathEnv), ';'))
		pToken = ";";
	else
		pToken = ":";

	pAct = strtok (pPathEnv, pToken);
	while (pAct)
	{
		snprintf(pTmpName, FILENAME_MAX, "%s%c%s",
		         pAct, PATHSEP, argv0);
		if (File_Exists(pTmpName))
		{
			/* Found the executable - so use the corresponding path: */
			strncpy(pExecDir, pAct, nMaxLen);
			pExecDir[nMaxLen-1] = 0;
			break;
		}
		pAct = strtok (0, pToken);
  	}

	free(pPathEnv);
	free(pTmpName);
}


/**
 * Locate the directory where the hatari executable resides
 */
static char *Paths_InitExecDir(const char *argv0)
{
	char *psExecDir;  /* Path string where the hatari executable can be found */

	/* Allocate memory for storing the path string of the executable */
	psExecDir = malloc(FILENAME_MAX);
	if (!psExecDir)
	{
		fprintf(stderr, "Out of memory (Paths_Init)\n");
		exit(-1);
	}

	/* Determine the bindir...
	 * Start with empty string, then try to use OS specific functions,
	 * and finally analyze the PATH variable if it has not been found yet. */
	psExecDir[0] = '\0';

#if defined(__linux__)
	{
		int i;
		/* On Linux, we can analyze the symlink /proc/self/exe */
		i = readlink("/proc/self/exe", psExecDir, FILENAME_MAX);
		if (i > 0)
		{
			char *p;
			psExecDir[i] = '\0';
			p = strrchr(psExecDir, '/');    /* Search last slash */
			if (p)
				*p = 0;                     /* Strip file name from path */
		}
	}
//#elif defined(WIN32) || defined(__CEGCC__)
//	/* On Windows we can use GetModuleFileName for getting the exe path */
//	GetModuleFileName(NULL, psExecDir, FILENAME_MAX);
#endif

	/* If we do not have the execdir yet, analyze argv[0] and the PATH: */
	if (psExecDir[0] == 0)
	{
		if (strchr(argv0, PATHSEP) == 0)
		{
			/* No separator in argv[0], we have to explore PATH... */
			Paths_GetExecDirFromPATH(argv0, psExecDir, FILENAME_MAX);
		}
		else
		{
			/* There was a path separator in argv[0], so let's assume a
			 * relative or absolute path to the current directory in argv[0] */
			char *p;
			strncpy(psExecDir, argv0, FILENAME_MAX);
			psExecDir[FILENAME_MAX-1] = 0;
			p = strrchr(psExecDir, PATHSEP);  /* Search last slash */
			if (p)
				*p = 0;                       /* Strip file name from path */
		}
	}

	return psExecDir;
}


/**
 * Initialize the users home directory string
 * and Hatari's home directory (~/.hatari)
 */
static void Paths_InitHomeDirs(void)
{
	char *psHome;

	psHome = getenv("HOME");
	if (psHome)
		strncpy(sUserHomeDir, psHome, FILENAME_MAX);
#if defined(WIN32)
	else
	{
		char *psDrive;
		int len = 0;
		/* Windows home path? */
		psHome = getenv("HOMEPATH");
		psDrive = getenv("HOMEDRIVE");
		if (psDrive)
		{
			len = strlen(psDrive);
			len = len < FILENAME_MAX ? len : FILENAME_MAX;
			strncpy(sUserHomeDir, psDrive, len);
		}
		if (psHome)
			strncpy(sUserHomeDir+len, psHome, FILENAME_MAX-len);
	}
#endif
	if (!psHome)
	{
		/* $HOME not set, so let's use current working dir as home */
		strcpy(sUserHomeDir, sWorkingDir);
		strcpy(sHatariHomeDir, sWorkingDir);
	}
	else
	{
		sUserHomeDir[FILENAME_MAX-1] = 0;

		/* Try to use a .hatari directory in the users home directory */
		snprintf(sHatariHomeDir, FILENAME_MAX, "%s%c.hatari",
		         sUserHomeDir, PATHSEP);
		if (!File_DirExists(sHatariHomeDir))
		{
			/* Hatari home directory does not exists yet...
			 * ...so let's try to create it: */
			if (mkdir(sHatariHomeDir, 0755) != 0)
			{
				/* Failed to create, so use user's home dir instead */
				strcpy(sHatariHomeDir, sUserHomeDir);
			}
		}
	}
}


/**
 * Initialize directory names
 *
 * The datadir will be initialized relative to the bindir (where the executable
 * has been installed to). This means a lot of additional effort since we first
 * have to find out where the executable is. But thanks to this effort, we get
 * a relocatable package (we don't have any absolute path names in the program)!
 */
void Paths_Init(const char *argv0)
{
	char *psExecDir;  /* Path string where the hatari executable can be found */

	/* Init working directory string */
	if (getcwd(sWorkingDir, FILENAME_MAX) == NULL)
	{
		/* This should never happen... just in case... */
		strcpy(sWorkingDir, ".");
	}

	/* Init the user's home directory string */
	Paths_InitHomeDirs();

	/* Get the directory where the executable resides */
	psExecDir = Paths_InitExecDir(argv0);

	/* Now create the datadir path name from the bindir path name: */
	if (psExecDir && strlen(psExecDir) > 0)
	{
		snprintf(sDataDir, sizeof(sDataDir), "%s%c%s",
		         psExecDir, PATHSEP, BIN2DATADIR);
	}
	else
	{
		/* bindir could not be determined, let's assume datadir is relative
		 * to current working directory... */
		strcpy(sDataDir, BIN2DATADIR);
	}

	/* And finally make a proper absolute path out of datadir: */
	File_MakeAbsoluteName(sDataDir);

	free(psExecDir);

	/* fprintf(stderr, " WorkingDir = %s\n DataDir = %s\n UserHomeDir = %s\n HatariHomeDir = %s\n",
	        sWorkingDir, sDataDir, sUserHomeDir, sHatariHomeDir); */
}
