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

#ifndef RETRO_DISK_CONTROL_H__
#define RETRO_DISK_CONTROL_H__

#include <stdbool.h>

//*****************************************************************************
// Disk control structure and functions
#define DC_MAX_SIZE 20

struct dc_storage{
	char* command;
	char* files[DC_MAX_SIZE];
	unsigned count;
	int index;
	bool eject_state;
};

typedef struct dc_storage dc_storage;
dc_storage* dc_create(void);
void dc_parse_m3u(dc_storage* dc, const char* m3u_file);
bool dc_add_file(dc_storage* dc, const char* filename);
void dc_free(dc_storage* dc);

#endif
