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

#include "retro_disk_control.h"
#include "retro_strings.h"
#include "retro_files.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <sys/types.h> 
#include <sys/stat.h> 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>*/

#define COMMENT "#"
#define M3U_SPECIAL_COMMAND "#COMMAND:"
#define M3U_SAVEDISK "#SAVEDISK:"

// Return the directory name of filename 'filename'.
char* dirname_int(const char* filename)
{
	if (filename == NULL)
		return NULL;
	
	char* right;
	int len = strlen(filename);
	
	if ((right = strrchr(filename, RETRO_PATH_SEPARATOR[0])) != NULL)
		return strleft(filename, len - strlen(right));
	
#ifdef _WIN32
	// Alternative search for windows beceause it also support the UNIX seperator
	if ((right = strrchr(filename, RETRO_PATH_SEPARATOR_ALT[0])) != NULL)
		return strleft(filename, len - strlen(right));
#endif
	
	// Not found
	return NULL;
}

char* m3u_search_file(const char* basedir, const char* dskName)
{
	// Verify if this item is an absolute pathname (or the file is in working dir)
	if (file_exists(dskName))
	{
		// Copy and return
		char* result = calloc(strlen(dskName) + 1, sizeof(char));
		strcpy(result, dskName);
		return result;
	}
	
	// If basedir was provided
	if(basedir != NULL)
	{
		// Join basedir and dskName
		char* dskPath = calloc(RETRO_PATH_MAX, sizeof(char));
		path_join(dskPath, basedir, dskName);

		// Verify if this item is a relative filename (append it to the m3u path)
		if (file_exists(dskPath))
		{
			// Return
			return dskPath;
		}
		free(dskPath);
	}
	
	// File not found
	return NULL;
}

void dc_reset(dc_storage* dc)
{
	// Verify
	if(dc == NULL)
		return;
	
	// Clean the command
	if(dc->command)
	{
		free(dc->command);
		dc->command = NULL;
	}

	// Clean the struct
	for(unsigned i=0; i < dc->count; i++)
	{
		free(dc->files[i]);
		dc->files[i] = NULL;
	}
	dc->count = 0;
	dc->index = -1;
	dc->eject_state = true;
}

dc_storage* dc_create(void)
{
	// Initialize the struct
	dc_storage* dc = NULL;
	
	if((dc = malloc(sizeof(dc_storage))) != NULL)
	{
		dc->count = 0;
		dc->index = -1;
		dc->eject_state = true;
		dc->command = NULL;
		for(int i = 0; i < DC_MAX_SIZE; i++)
		{
			dc->files[i] = NULL;
		}
	}
	
	return dc;
}

bool dc_add_file_int(dc_storage* dc, char* filename)
{
	// Verify
	if(dc == NULL)
		return false;

	if(filename == NULL)
		return false;

	// If max size is not
	if(dc->count < DC_MAX_SIZE)
	{
		// Add the file
		dc->count++;
		dc->files[dc->count-1] = filename;
		return true;
	}
	
	return false;
}

bool dc_add_file(dc_storage* dc, const char* filename)
{
	// Verify
	if(dc == NULL)
		return false;

	if(filename == NULL)
		return false;

	// Copy and return
	char* filename_int = calloc(strlen(filename) + 1, sizeof(char));
	strcpy(filename_int, filename);
	return dc_add_file_int(dc, filename_int);
}

static bool dc_add_m3u_save_disk(
		dc_storage* dc,
		const char* m3u_file, const char* save_dir,
		const char* disk_name, unsigned int index)
{
	bool save_disk_exists                     = false;
	char *m3u_file_name                       = NULL;
	char *last_period                         = NULL;
	char m3u_file_name_no_ext[RETRO_PATH_MAX] = {0};
	char save_disk_file_name[RETRO_PATH_MAX]  = {0};
	char save_disk_path[RETRO_PATH_MAX]       = {0};
	char volume_name[31]                      = {0};
	
	// Verify
	if(dc == NULL)
		return false;

	if(m3u_file == NULL)
		return false;
	
	if(save_dir == NULL)
		return false;
	
	// It seems that we don't want to use "string/stdstring.h"
	// or "file/file_path.h" functions here, so this will be ugly...
	
	// Get m3u file name
	m3u_file_name = strrchr(m3u_file, RETRO_PATH_SEPARATOR[0]);
#ifdef _WIN32
	if (m3u_file_name == NULL)
		m3u_file_name = strrchr(m3u_file, RETRO_PATH_SEPARATOR_ALT[0]);
#endif
	
	if (!m3u_file_name || (*m3u_file_name == '\0'))
		return false;
	
	m3u_file_name++;
	
	if (*m3u_file_name == '\0')
		return false;
	
	// Get m3u file name without extension
	// (Looks like we can't use strlcpy() either...)
	strncpy(m3u_file_name_no_ext, m3u_file_name, sizeof(m3u_file_name_no_ext) - 1);
	
	last_period = strrchr(m3u_file_name_no_ext, '.');
	if (last_period != NULL)
		*last_period = '\0';
	
	// Construct save disk file name
	snprintf(save_disk_file_name, RETRO_PATH_MAX, "%s.save%u.adf",
			m3u_file_name_no_ext, index);
	
	// Construct save disk path
	path_join(save_disk_path, save_dir, save_disk_file_name);
	
	// Check whether save disk already exists
	// Note: If a disk already exists, we should be
	// able to support changing the volume label if
	// it differs from 'disk_name'. This is quite
	// fiddly, however - perhaps it can be added later...
	save_disk_exists = file_exists(save_disk_path);
	
	// ...if not, create a new one
	if (!save_disk_exists)
	{
		// Get volume name
		// > If disk_name is NULL or empty/EMPTY,
		//   no volume name is set
		if (disk_name && (*disk_name != '\0'))
		{
			if(strncasecmp(disk_name, "empty", strlen("empty")))
			{
				char *scrub_pointer = NULL;
				
				// Ensure volume name is valid
				// > Must be <= 30 characters
				// > Cannot contain '/' or ':'
				strncpy(volume_name, disk_name, sizeof(volume_name) - 1);
				
				while((scrub_pointer = strpbrk(volume_name, "/:")))
					*scrub_pointer = '_';
			}
		}
		
		// Create save disk
		save_disk_exists = disk_creatediskfile(
				save_disk_path, 0, DRV_35_DD,
				(volume_name[0] == '\0') ? NULL : volume_name,
				false, false, NULL);
	}
	
	// If save disk exists/was created, add it to
	// the list
	if (save_disk_exists)
	{
		dc_add_file_int(dc, my_strdup(save_disk_path));
		return true;
	}
	
	return false;
}

void dc_parse_m3u(dc_storage* dc, const char* m3u_file, const char* save_dir)
{
	// Verify
	if(dc == NULL)
		return;
	
	if(m3u_file == NULL)
		return;

	FILE* fp = NULL;

	// Try to open the file
	if ((fp = fopen(m3u_file, "r")) == NULL)
		return;

	// Reset
	dc_reset(dc);
	
	// Get the m3u base dir for resolving relative path
	char* basedir = dirname_int(m3u_file);
	
	// Read the lines while there is line to read and we have enough space
	unsigned int save_disk_index = 0;
	char buffer[2048];
	while ((dc->count <= DC_MAX_SIZE) && (fgets(buffer, sizeof(buffer), fp) != NULL))
	{
		char* string = trimwhitespace(buffer);
		
		// If it's a m3u special key or a file
		if (strstartswith(string, M3U_SPECIAL_COMMAND))
		{
			dc->command = strright(string, strlen(string) - strlen(M3U_SPECIAL_COMMAND));
		}
		else if (strstartswith(string, M3U_SAVEDISK))
		{
			// Get volume name
			char* disk_name = strright(string, strlen(string) - strlen(M3U_SAVEDISK));
			
			// Add save disk, creating it if necessary
			if (dc_add_m3u_save_disk(
					dc, m3u_file, save_dir,
					disk_name, save_disk_index))
				save_disk_index++;
			
			// Clean up
			if (disk_name != NULL)
				free(disk_name);
		}
		else if (!strstartswith(string, COMMENT))
		{
			// Search the file (absolute, relative to m3u)
			char* filename;
			if ((filename = m3u_search_file(basedir, string)) != NULL)
			{
				// Add the file to the struct
				dc_add_file_int(dc, filename);
			}
		}
	}
	
	// If basedir was provided
	if(basedir != NULL)
		free(basedir);

	// Close the file 
	fclose(fp);
}

void dc_free(dc_storage* dc)
{
	// Clean the struct
	dc_reset(dc);
	free(dc);
	dc = NULL;
	return;
}
