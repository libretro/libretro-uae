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
#include "file/file_path.h"
#include "libretro-glue.h"
#include "retroglue.h"

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
#define M3U_SAVEDISK_LABEL "SAVE DISK"
#define M3U_PATH_DELIM '|'

extern char retro_save_directory[RETRO_PATH_MAX];
extern char retro_temp_directory[RETRO_PATH_MAX];

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
		if (dc->files[i])
			free(dc->files[i]);
		dc->files[i] = NULL;

		if (dc->labels[i])
			free(dc->labels[i]);
		dc->labels[i] = NULL;

		dc->types[i] = DC_IMAGE_TYPE_NONE;
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
			dc->files[i]  = NULL;
			dc->labels[i] = NULL;
			dc->types[i]  = DC_IMAGE_TYPE_NONE;
		}
	}
	
	return dc;
}

bool dc_add_file_int(dc_storage* dc, char* filename, char* label)
{
	// Verify
	if (dc == NULL)
		return false;

	if (!filename || (*filename == '\0'))
		return false;

	// If max size is not exceeded...
	if(dc->count < DC_MAX_SIZE)
	{
		// Add the file
		dc->count++;
		dc->files[dc->count-1]  = filename;
		dc->labels[dc->count-1] = label;
		dc->types[dc->count-1]  = dc_get_image_type(filename);
		return true;
	}
	
	return false;
}

bool dc_add_file(dc_storage* dc, const char* filename, const char* label)
{
	// Verify
	if(dc == NULL)
		return false;

	if (!filename || (*filename == '\0'))
		return false;

	// Copy and return
	char *filename_int = calloc(strlen(filename) + 1, sizeof(char));
	strcpy(filename_int, filename);

	char *label_int = NULL; // NULL is a valid label
	if (!(!label || (*label == '\0')))
	{
		label_int = calloc(strlen(label) + 1, sizeof(char));
		strcpy(label_int, label);
	}

	return dc_add_file_int(dc, filename_int, label_int);
}

static bool dc_add_m3u_save_disk(
		dc_storage* dc,
		const char* m3u_file, const char* save_dir,
		const char* disk_name, unsigned int index)
{
	bool save_disk_exists                     = false;
	const char *m3u_file_name                 = NULL;
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
	m3u_file_name = path_get_basename(m3u_file);
	
	if (!m3u_file_name || (*m3u_file_name == '\0'))
		return false;
	
	// Get m3u file name without extension
	remove_file_extension(
			m3u_file_name, m3u_file_name_no_ext, sizeof(m3u_file_name_no_ext));
	
	if (!m3u_file_name_no_ext || (*m3u_file_name_no_ext == '\0'))
		return false;
	
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
		char save_disk_label[64] = {0};
		
		snprintf(save_disk_label, 64, "%s %u",
				M3U_SAVEDISK_LABEL, index);
		
		dc_add_file_int(dc, my_strdup(save_disk_path), my_strdup(save_disk_label));
		return true;
	}
	
	return false;
}

static bool dc_add_m3u_disk(
		dc_storage* dc, const char *m3u_base_dir,
		const char* disk_file, const char* usr_disk_label,
		bool usr_disk_label_set)
{
	char *disk_file_path = NULL;
	
	// Verify
	if(dc == NULL)
		return false;
	
	if(m3u_base_dir == NULL)
		return false;
	
	if(disk_file == NULL)
		return false;
	
	// Search the file (absolute, relative to m3u)
	if ((disk_file_path = m3u_search_file(m3u_base_dir, disk_file)) != NULL)
	{
		char disk_label[RETRO_PATH_MAX] = {0};
		
		// If a label is provided, use it
		if (usr_disk_label_set)
		{
			// Note that label may intentionally be left blank
			if (usr_disk_label && (*usr_disk_label != '\0'))
				strncpy(
						disk_label, usr_disk_label, sizeof(disk_label) - 1);
		}
		else
		{
			// Otherwise, use file name without extension as label
			const char *file_name = path_get_basename(disk_file_path);
			
			if (!(!file_name || (*file_name == '\0')))
			{
				remove_file_extension(
						file_name, disk_label, sizeof(disk_label));
			}
		}

		// ZIP
		if (strendswith(disk_file_path, "zip"))
		{
			char zip_basename[RETRO_PATH_MAX];
			snprintf(zip_basename, sizeof(zip_basename), "%s", path_basename(disk_file_path));
			snprintf(zip_basename, sizeof(zip_basename), "%s", path_remove_extension(zip_basename));
			snprintf(retro_temp_directory, sizeof(retro_temp_directory), "%s%s%s", retro_save_directory, DIR_SEP_STR, "ZIP");
			char zip_path[RETRO_PATH_MAX];
			snprintf(zip_path, sizeof(zip_path), "%s%s%s", retro_temp_directory, DIR_SEP_STR, zip_basename);
			char lastfile[RETRO_PATH_MAX];

			path_mkdir(zip_path);
			zip_uncompress(disk_file_path, zip_path, lastfile);
			snprintf(disk_file_path, RETRO_PATH_MAX, "%s%s%s", zip_path, DIR_SEP_STR, lastfile);
		}
		
		// Add the file to the list
		dc_add_file_int(
				dc, disk_file_path,
				(disk_label[0] == '\0') ? NULL : my_strdup(disk_label));
		
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
			// Path format:
			//    FILE_NAME|FILE_LABEL
			// Delimiter + FILE_LABEL is optional
			char file_name[RETRO_PATH_MAX]  = {0};
			char file_label[RETRO_PATH_MAX] = {0};
			char* delim_ptr                 = strchr(string, M3U_PATH_DELIM);
			bool label_set                  = false;
			
			if (delim_ptr)
			{
				// Not going to use strleft()/strright() here,
				// since these functions allocate new strings,
				// which we don't want to do...
				
				// Get FILE_NAME segment
				size_t len = (size_t)(1 + delim_ptr - string);
				if (len > 0)
					strncpy(
							file_name, string,
							((len < RETRO_PATH_MAX ? len : RETRO_PATH_MAX) * sizeof(char)) - 1);
				
				// Get FILE_LABEL segment
				delim_ptr++;
				if (*delim_ptr != '\0')
					strncpy(
							file_label, delim_ptr, sizeof(file_label) - 1);

				// Note: If delimiter is present but FILE_LABEL
				// is omitted, label is intentionally left blank
				label_set = true;
			}
			else
				strncpy(file_name, string, sizeof(file_name) - 1);

			dc_add_m3u_disk(
					dc, basedir,
					trimwhitespace(file_name),
					(file_label[0] == '\0') ? NULL : trimwhitespace(file_label),
					label_set);
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

enum dc_image_type dc_get_image_type(const char* filename)
{
	// Missing file
	if (!filename || (*filename == '\0'))
	   return DC_IMAGE_TYPE_NONE;

	// Floppy image
	if (strendswith(filename, "adf") ||
	    strendswith(filename, "adz") ||
	    strendswith(filename, "fdi") ||
	    strendswith(filename, "dms") ||
	    strendswith(filename, "ipf") ||
	    strendswith(filename, "zip"))
	   return DC_IMAGE_TYPE_FLOPPY;

	// CD image
	if (strendswith(filename, "cue") ||
	    strendswith(filename, "ccd") ||
	    strendswith(filename, "nrg") ||
	    strendswith(filename, "mds") ||
	    strendswith(filename, "iso"))
	   return DC_IMAGE_TYPE_CD;

	// HD image
	if (strendswith(filename, "hdf") ||
	    strendswith(filename, "hdz") ||
	    path_is_directory(filename))
	   return DC_IMAGE_TYPE_HD;

	// WHDLoad
	if (strendswith(filename, "lha") ||
	    strendswith(filename, "slave") ||
	    strendswith(filename, "info"))
	   return DC_IMAGE_TYPE_WHDLOAD;

	// Fallback
	return DC_IMAGE_TYPE_UNKNOWN;
}
