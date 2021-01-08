#ifndef LIBRETRO_GLUE_H
#define LIBRETRO_GLUE_H

#include <stdio.h>

/* zlib */
#include "deps/libz/zlib.h"
#include "deps/libz/unzip.h"
void gz_uncompress(gzFile in, FILE *out);
void zip_uncompress(char *in, char *out, char *lastfile);

/* 7z */
#include "deps/7zip/7z.h"
#include "deps/7zip/7zBuf.h"
#include "deps/7zip/7zCrc.h"
#include "deps/7zip/7zFile.h"
#include "deps/7zip/7zTypes.h"
void sevenzip_uncompress(char *in, char *out, char *lastfile);

/* HDF */
int make_hdf (char *hdf_path, char *hdf_size, char *device_name);

/* Misc */
int qstrcmp(const void *a, const void *b);
void remove_recurse(const char *path);
int fcopy(const char *src, const char *dst);

/* String helpers functions */
char* trimwhitespace(char *str);
char* strleft(const char* str, int len);
char* strright(const char* str, int len);
bool strstartswith(const char* str, const char* start);
bool strendswith(const char* str, const char* end);
void path_join(char* out, const char* basedir, const char* filename);

#endif /* LIBRETRO_GLUE_H */
