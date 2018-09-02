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

#ifndef RETRO_FILES_H__
#define RETRO_FILES_H__

#include <stdbool.h>

//*****************************************************************************
// File helpers functions
#define RETRO_PATH_MAX 512

#ifdef _WIN32
#define RETRO_PATH_SEPARATOR   		"\\"
// Windows also support the unix path separator
#define RETRO_PATH_SEPARATOR_ALT   	"/"
#else
#define RETRO_PATH_SEPARATOR   		"/"
#endif

void path_join(char* out, const char* basedir, const char* filename);
bool file_exists(const char *filename);

#endif
