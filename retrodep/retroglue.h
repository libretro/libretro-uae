#ifndef _RETRO_GLUE_H_
#define _RETRO_GLUE_H_

#include "deps/zlib/zlib.h"
void gz_uncompress(gzFile in, void *out);

#include "deps/zlib/unzip.h"
void zip_uncompress(char *in, char *out);

#endif


