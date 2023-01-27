/* Copyright (C) 2018 
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LIBRETRO_DC_H
#define LIBRETRO_DC_H

#include <stdbool.h>

#define COMMENT             "#"
#define M3U_SPECIAL_COMMAND "#COMMAND:"
#define M3U_SAVEDISK        "#SAVEDISK:"
#define M3U_SAVEDISK_LABEL  "Save Disk"
#define M3U_PATH_DELIM      '|'

/* Disk control structure and functions */
#define DC_MAX_SIZE 20

enum dc_image_type
{
   DC_IMAGE_TYPE_NONE = 0,
   DC_IMAGE_TYPE_FLOPPY,
   DC_IMAGE_TYPE_CD,
   DC_IMAGE_TYPE_HD,
   DC_IMAGE_TYPE_WHDLOAD,
   DC_IMAGE_TYPE_ARCHIVE,
   DC_IMAGE_TYPE_UNKNOWN
};

struct dc_storage
{
   char* command;
   char* files[DC_MAX_SIZE];
   char* labels[DC_MAX_SIZE];
   enum dc_image_type types[DC_MAX_SIZE];
   unsigned count;
   int index;
   int index_prev;
   bool eject_state;
   bool replace;
};

typedef struct dc_storage dc_storage;
dc_storage* dc_create(void);
void dc_parse_m3u(dc_storage* dc, const char* m3u_file, const char* save_dir);
unsigned dc_inspect_m3u(const char* m3u_file);
bool dc_add_file(dc_storage* dc, const char* filename, const char* label);
void dc_free(dc_storage* dc);
void dc_reset(dc_storage* dc);
bool dc_replace_file(dc_storage* dc, int index, const char* filename);
bool dc_remove_file(dc_storage* dc, int index);
enum dc_image_type dc_get_image_type(const char* filename);
bool dc_save_disk_toggle(dc_storage* dc, bool file_check, bool select);
void dc_save_disk_compress(dc_storage* dc);

#endif /* LIBRETRO_DC_H */
