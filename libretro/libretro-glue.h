#ifndef LIBRETRO_GLUE_H
#define LIBRETRO_GLUE_H

/* zlib */
#include "deps/zlib/zlib.h"
#include <stdio.h>
void gz_uncompress(gzFile in, FILE *out);

#include "deps/zlib/unzip.h"
void zip_uncompress(char *in, char *out, char *lastfile);

/* HDF */
int make_hdf (char *hdf_path, char *hdf_size, char *device_name);

/* Misc */
int qstrcmp(const void *a, const void *b);
void remove_recurse(const char *path);

/* String helpers functions */
char* trimwhitespace(char *str);
char* strleft(const char* str, int len);
char* strright(const char* str, int len);
bool strstartswith(const char* str, const char* start);
bool strendswith(const char* str, const char* end);
void path_join(char* out, const char* basedir, const char* filename);

#endif /* LIBRETRO_GLUE_H */
