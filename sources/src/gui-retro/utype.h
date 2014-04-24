/*
	modded for libretro-uae
*/

#ifndef HATARI_UTYPE_H
#define HATARI_UTYPE_H

typedef unsigned char	Uint8;
typedef signed char	Sint8;
typedef unsigned short	Uint16;
typedef signed short	Sint16;
typedef unsigned int	Uint32;
typedef signed int	Sint32;

typedef signed char     int8;
typedef signed short    int16;
typedef signed int   int32;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int  uint32;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//RETO HACK
#ifdef RETRO
#include <ctype.h>

#ifdef PS3PORT
#include <sdk_version.h>
#include <cell.h>
#include <stdio.h>
#include <string.h>
#define	getcwd(a,b)	"/"
#include <unistd.h> //stat() is defined here
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#define F_OK 0
#define ftello ftell
#define chdir(a) 0
#define getenv(a)	"/dev_hdd0/HOMEBREW/ST/"
#define tmpfile() NULL
#endif
//#define PROG_NAME "Hatari devel (" __DATE__ ")"
#endif

#ifdef WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

#endif

