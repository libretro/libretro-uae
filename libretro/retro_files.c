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

#include "retro_files.h"

#include <sys/stat.h> 
#include <stdio.h>
#include <string.h>
#ifdef VITA
#include <file/file_path.h>
#endif

// Verify if file exists
bool file_exists(const char *filename)
{
	struct stat buf;
#ifdef VITA
	if (path_is_valid(filename) && !path_is_directory(filename))
#else
	if (stat(filename, &buf) == 0 &&
	    (buf.st_mode & (S_IRUSR|S_IWUSR)) && !(buf.st_mode & S_IFDIR))
#endif
	{
		/* file points to user readable regular file */
		return true;
	}
	return false;
}

void path_join(char* out, const char* basedir, const char* filename)
{
	snprintf(out, RETRO_PATH_MAX, "%s%s%s", basedir, RETRO_PATH_SEPARATOR, filename);
}

const char *path_get_basename(const char* path)
{
	char *basename = NULL;

	if (!path || (*path == '\0'))
		return NULL;

	// Search for path separator
	basename = strrchr(path, RETRO_PATH_SEPARATOR[0]);
#ifdef _WIN32
	if (!basename)
		basename = strrchr(path, RETRO_PATH_SEPARATOR_ALT[0]);
#endif

	if (!(!basename || (*basename == '\0')))
		return basename + 1;

	// > If path contains no separator, then the
	//   path itself is the basename
	return path;
}

void remove_file_extension(const char* path, char* path_no_ext, size_t len)
{
	char *last_period = NULL;

	if (!path || (*path == '\0'))
		return;

	if (len < 1)
		return;

	strncpy(path_no_ext, path, len - 1);

	last_period = strrchr(path_no_ext, '.');
	if (last_period != NULL)
		*last_period = '\0';
}
