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

#include "libretro-dc.h"
#include "libretro-core.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMENT "#"
#define M3U_SPECIAL_COMMAND "#COMMAND:"
#define M3U_SAVEDISK "#SAVEDISK:"
#define M3U_SAVEDISK_LABEL "Save Disk"
#define M3U_PATH_DELIM '|'

extern char retro_save_directory[RETRO_PATH_MAX];
extern char retro_temp_directory[RETRO_PATH_MAX];
extern bool retro_disk_set_image_index(unsigned index);
extern bool opt_floppy_multidrive;
extern char full_path[RETRO_PATH_MAX];
extern void display_current_image(const char *image, bool inserted);

/* Return the directory name of filename 'filename' */
char* dirname_int(const char* filename)
{
   if (filename == NULL)
      return NULL;

   char* right;
   int len = strlen(filename);

   if ((right = strrchr(filename, RETRO_PATH_SEPARATOR[0])) != NULL)
      return strleft(filename, len - strlen(right));

#ifdef _WIN32
   /* Alternative search for windows beceause it also support the UNIX seperator */
   if ((right = strrchr(filename, RETRO_PATH_SEPARATOR_ALT[0])) != NULL)
      return strleft(filename, len - strlen(right));
#endif

   /* Not found */
   return NULL;
}

char* m3u_search_file(const char* basedir, const char* dskName)
{
   /* Verify if this item is an absolute pathname (or the file is in working dir) */
   if (path_is_valid(dskName))
   {
      /* Copy and return */
      char* result = calloc(strlen(dskName) + 1, sizeof(char));
      strcpy(result, dskName);
      return result;
   }

   /* If basedir was provided */
   if(basedir != NULL)
   {
      /* Join basedir and dskName */
      char* dskPath = calloc(RETRO_PATH_MAX, sizeof(char));
      path_join(dskPath, basedir, dskName);

      /* Verify if this item is a relative filename (append it to the m3u path) */
      if (path_is_valid(dskPath))
      {
         /* Return */
         return dskPath;
      }
      free(dskPath);
   }

   /* File not found */
   return NULL;
}

void dc_reset(dc_storage* dc)
{
   /* Verify */
   if(dc == NULL)
      return;

   /* Clean the command */
   if(dc->command)
   {
      free(dc->command);
      dc->command = NULL;
   }

   /* Clean the struct */
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
   dc->index = 0;
   dc->eject_state = true;
   dc->replace = false;
}

dc_storage* dc_create(void)
{
   /* Initialize the struct */
   dc_storage* dc = NULL;

   if((dc = malloc(sizeof(dc_storage))) != NULL)
   {
      dc->count = 0;
      dc->index = -1;
      dc->eject_state = true;
      dc->replace = false;
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
   /* Verify */
   if (dc == NULL)
      return false;

   if (!filename || (*filename == '\0'))
      return false;

   /* If max size is not exceeded */
   if(dc->count < DC_MAX_SIZE)
   {
      /* Add the file */
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
   /* Verify */
   if(dc == NULL)
      return false;

   if (!filename || (*filename == '\0'))
      return false;

   return dc_add_file_int(dc, my_strdup(filename), my_strdup(label));
}

bool dc_remove_file(dc_storage* dc, int index)
{
    if (dc == NULL)
        return false;

    if (index < 0 || index >= dc->count)
        return false;

    /* "If ptr is a null pointer, no action occurs" */
    free(dc->files[index]);
    dc->files[index] = NULL;
    free(dc->labels[index]);
    dc->labels[index] = NULL;
    dc->types[index] = DC_IMAGE_TYPE_NONE;

    /* Shift all entries after index one slot up */
    if (index != dc->count - 1)
    {
        memmove(dc->files + index, dc->files + index + 1, (dc->count - 1 - index) * sizeof(dc->files[0]));
        memmove(dc->labels + index, dc->labels + index + 1, (dc->count - 1 - index) * sizeof(dc->labels[0]));
    }

    dc->count--;

    return true;
}

bool dc_replace_file(dc_storage* dc, int index, const char* filename)
{
    if (dc == NULL)
        return false;

    if (index < 0 || index >= dc->count)
        return false;

    /* "If ptr is a null pointer, no action occurs" */
    free(dc->files[index]);
    dc->files[index] = NULL;
    free(dc->labels[index]);
    dc->labels[index] = NULL;
    dc->types[index] = DC_IMAGE_TYPE_NONE;

    if (filename == NULL)
        dc_remove_file(dc, index);
    else
    {
        dc->replace = false;

        /* Eject all floppy drives */
        for (unsigned i = 0; i < 4; i++)
            changed_prefs.floppyslots[i].df[0] = 0;

        char full_path_replace[RETRO_PATH_MAX] = {0};
        strcpy(full_path_replace, (char*)filename);

        /* Confs & hard drive images will replace full_path and requires restarting */
        if (strendswith(full_path_replace, "uae")
         || strendswith(full_path_replace, "hdf")
         || strendswith(full_path_replace, "hdz")
         || strendswith(full_path_replace, "lha"))
        {
            dc_reset(dc);
            strcpy(full_path, (char*)filename);
            display_current_image(full_path, true);
            return false;
        }

        /* ZIP */
        else if (strendswith(full_path_replace, "zip"))
        {
            char zip_basename[RETRO_PATH_MAX] = {0};
            snprintf(zip_basename, sizeof(zip_basename), "%s", path_basename(full_path_replace));
            snprintf(zip_basename, sizeof(zip_basename), "%s", path_remove_extension(zip_basename));

            path_mkdir(retro_temp_directory);
            zip_uncompress(full_path_replace, retro_temp_directory, NULL);

            /* Default to directory mode */
            int zip_mode = 0;
            snprintf(full_path_replace, sizeof(full_path_replace), "%s", retro_temp_directory);

            FILE *zip_m3u;
            char zip_m3u_list[DC_MAX_SIZE][RETRO_PATH_MAX] = {0};
            char zip_m3u_path[RETRO_PATH_MAX] = {0};
            snprintf(zip_m3u_path, sizeof(zip_m3u_path), "%s%s%s.m3u", retro_temp_directory, DIR_SEP_STR, zip_basename);
            int zip_m3u_num = 0;

            DIR *zip_dir;
            struct dirent *zip_dirp;
            zip_dir = opendir(retro_temp_directory);
            while ((zip_dirp = readdir(zip_dir)) != NULL)
            {
                if (zip_dirp->d_name[0] == '.' || strendswith(zip_dirp->d_name, "m3u") || zip_mode > 1)
                    continue;

                /* Multi file mode, generate playlist */
                if (dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_FLOPPY)
                {
                    zip_mode = 1;
                    zip_m3u_num++;
                    snprintf(zip_m3u_list[zip_m3u_num-1], RETRO_PATH_MAX, "%s", zip_dirp->d_name);
                }
                /* Single file image mode */
                else if (dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_CD
                      || dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_HD)
                {
                    zip_mode = 2;
                    snprintf(full_path_replace, sizeof(full_path_replace), "%s%s%s", retro_temp_directory, DIR_SEP_STR, zip_dirp->d_name);
                }
            }
            closedir(zip_dir);

            switch (zip_mode)
            {
                case 0: /* Extracted path */
                    dc_reset(dc);
                    strcpy(full_path, (char*)filename);
                    display_current_image(full_path, true);
                    return true;
                    break;
                case 2: /* Single image */
                    break;
                case 1: /* Generated playlist */
                    if (zip_m3u_num == 1)
                    {
                        snprintf(full_path_replace, sizeof(full_path_replace), "%s%s%s", retro_temp_directory, DIR_SEP_STR, zip_m3u_list[0]);
                    }
                    else
                    {
                        zip_m3u = fopen(zip_m3u_path, "w");
                        qsort(zip_m3u_list, zip_m3u_num, RETRO_PATH_MAX, qstrcmp);
                        for (int l = 0; l < zip_m3u_num; l++)
                            fprintf(zip_m3u, "%s\n", zip_m3u_list[l]);
                        fclose(zip_m3u);
                        snprintf(full_path_replace, sizeof(full_path_replace), "%s", zip_m3u_path);
                    }
                    break;
            }
        }

        /* M3U replace */
        if (strendswith(full_path_replace, "m3u"))
        {
            /* Parse the M3U file */
            dc_parse_m3u(dc, full_path_replace, retro_save_directory);

            /* Some debugging */
            log_cb(RETRO_LOG_INFO, "M3U parsed, %d file(s) found\n", dc->count);
            for (unsigned i = 0; i < dc->count; i++)
                log_cb(RETRO_LOG_DEBUG, "File %d: %s\n", i+1, dc->files[i]);

            /* Insert first disk */
            retro_disk_set_image_index(0);

            /* Trick frontend to return to index 0 after successful "append" does +1 */
            dc->replace = true;

            /* Append rest of the disks to the config if M3U is a MultiDrive-M3U */
            if (strstr(full_path_replace, "(MD)") != NULL || opt_floppy_multidrive)
            {
                for (unsigned i = 1; i < dc->count; i++)
                {
                    if (i < 4)
                    {
                        log_cb(RETRO_LOG_INFO, "Disk (%d) inserted in drive DF%d: '%s'\n", i+1, i, dc->files[i]);
                        strcpy(changed_prefs.floppyslots[i].df, dc->files[i]);

                        /* By default only DF0: is enabled, so floppyXtype needs to be set on the extra drives */
                        changed_prefs.floppyslots[i].dfxtype = 0; /* 0 = 3.5" DD */
                    }
                    else
                    {
                        log_cb(RETRO_LOG_WARN, "Too many disks for MultiDrive!\n");
                        snprintf(retro_message_msg, sizeof(retro_message_msg), "Too many disks for MultiDrive!");
                        retro_message = true;
                    }
                }
            }
        }
        /* Single append */
        else
        {
            char image_label[RETRO_PATH_MAX];
            image_label[0] = '\0';
            fill_short_pathname_representation(image_label, full_path_replace, sizeof(image_label));

            dc->files[index]  = strdup(full_path_replace);
            dc->labels[index] = strdup(image_label);
            dc->types[index]  = dc_get_image_type(full_path_replace);
        }
    }

    return true;
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

   /* Verify */
   if(dc == NULL)
      return false;

   if(m3u_file == NULL)
      return false;

   if(save_dir == NULL)
      return false;

   /* It seems that we don't want to use "string/stdstring.h"
    * or "file/file_path.h" functions here, so this will be ugly */

   /* Get m3u file name */
   m3u_file_name = path_basename(m3u_file);
   if (!m3u_file_name || (*m3u_file_name == '\0'))
      return false;

   /* Get m3u file name without extension */
    snprintf(m3u_file_name_no_ext, sizeof(m3u_file_name_no_ext),
             "%s", path_remove_extension((char*)m3u_file_name));

   if (!m3u_file_name_no_ext || (*m3u_file_name_no_ext == '\0'))
      return false;

   /* Construct save disk file name */
   snprintf(save_disk_file_name, RETRO_PATH_MAX, "%s.save%u.adf",
         m3u_file_name_no_ext, index);

   /* Construct save disk path */
   path_join(save_disk_path, save_dir, save_disk_file_name);

   /* Check whether save disk already exists
    * Note: If a disk already exists, we should be
    * able to support changing the volume label if
    * it differs from 'disk_name'. This is quite
    * fiddly, however - perhaps it can be added later... */
   save_disk_exists = path_is_valid(save_disk_path);

   /* ...if not, create a new one */
   if (!save_disk_exists)
   {
      /* Get volume name
       * > If disk_name is NULL or empty/EMPTY,
       *   no volume name is set */
      if (disk_name && (*disk_name != '\0'))
      {
         if(strncasecmp(disk_name, "empty", strlen("empty")))
         {
            char *scrub_pointer = NULL;

            /* Ensure volume name is valid
             * > Must be <= 30 characters
             * > Cannot contain '/' or ':' */
            strncpy(volume_name, disk_name, sizeof(volume_name) - 1);

            while((scrub_pointer = strpbrk(volume_name, "/:")))
               *scrub_pointer = '_';
         }
      }

      /* Set empty disk label as visible label */
      if (string_is_empty(volume_name))
          snprintf(volume_name, sizeof(volume_name), "%s %u",
                   M3U_SAVEDISK_LABEL, index);

      /* Create save disk */
      save_disk_exists = disk_creatediskfile(
            save_disk_path, 0, DRV_35_DD,
            (volume_name[0] == '\0') ? NULL : volume_name,
            false, false, NULL);
   }

   /* If save disk exists/was created, add it to the list */
   if (save_disk_exists)
   {
      char save_disk_label[64] = {0};

      snprintf(save_disk_label, 64, "%s %u",
            M3U_SAVEDISK_LABEL, index);

      dc_add_file(dc, save_disk_path, save_disk_label);
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

   /* Verify */
   if(dc == NULL)
      return false;

   if(m3u_base_dir == NULL)
      return false;

   if(disk_file == NULL)
      return false;

   /* "Browsed" file in ZIP */
   char browsed_file[RETRO_PATH_MAX] = {0};
   if (strstr(disk_file, ".zip#"))
   {
      char *token = strtok((char*)disk_file, "#");
      while (token != NULL)
      {
         snprintf(browsed_file, sizeof(browsed_file), "%s", token);
         token = strtok(NULL, "#");
      }
   }

   /* Search the file (absolute, relative to m3u) */
   if ((disk_file_path = m3u_search_file(m3u_base_dir, disk_file)) != NULL)
   {
      char disk_label[RETRO_PATH_MAX] = {0};
      char full_path[RETRO_PATH_MAX] = {0};
      snprintf(full_path, sizeof(full_path), "%s", disk_file_path);

      /* If a label is provided, use it */
      if (usr_disk_label_set)
      {
         /* Note that label may intentionally be left blank */
         if (usr_disk_label && (*usr_disk_label != '\0'))
            strncpy(disk_label, usr_disk_label, sizeof(disk_label) - 1);
      }
      else
      {
         /* Otherwise, use file name without extension as label */
         const char *file_name = path_basename(disk_file_path);
         if (!string_is_empty(browsed_file))
             file_name = path_basename(browsed_file);

         if (!(!file_name || (*file_name == '\0')))
                snprintf(disk_label, sizeof(disk_label),
                      "%s", path_remove_extension((char*)file_name));
      }

      /* ZIP */
      if (strendswith(disk_file_path, "zip"))
      {
         char lastfile[RETRO_PATH_MAX];
         char zip_basename[RETRO_PATH_MAX];
         snprintf(zip_basename, sizeof(zip_basename), "%s", path_basename(full_path));
         snprintf(zip_basename, sizeof(zip_basename), "%s", path_remove_extension(zip_basename));

         path_mkdir(retro_temp_directory);
         zip_uncompress(full_path, retro_temp_directory, lastfile);

         if (!string_is_empty(browsed_file))
             snprintf(lastfile, sizeof(lastfile), "%s", browsed_file);
         snprintf(full_path, RETRO_PATH_MAX, "%s%s%s", retro_temp_directory, DIR_SEP_STR, lastfile);
      }

      /* Add the file to the list */
      if (path_is_valid(full_path))
          dc_add_file(
            dc, full_path,
            (disk_label[0] == '\0') ? NULL : disk_label);

      return true;
   }

   return false;
}

void dc_parse_m3u(dc_storage* dc, const char* m3u_file, const char* save_dir)
{
   /* Verify */
   if(dc == NULL)
      return;

   if(m3u_file == NULL)
      return;

   FILE* fp = NULL;

   /* Try to open the file */
   if ((fp = fopen(m3u_file, "r")) == NULL)
      return;

   /* Reset */
   dc_reset(dc);

   /* Get the m3u base dir for resolving relative path */
   char* basedir = dirname_int(m3u_file);

   /* Read the lines while there is line to read and we have enough space */
   unsigned int save_disk_index = 0;
   char buffer[2048];
   while ((dc->count <= DC_MAX_SIZE) && (fgets(buffer, sizeof(buffer), fp) != NULL))
   {
      char* string = trimwhitespace(buffer);

      /* If it's a m3u special key or a file */
      if (strstartswith(string, M3U_SPECIAL_COMMAND))
      {
         dc->command = strright(string, strlen(string) - strlen(M3U_SPECIAL_COMMAND));
      }
      else if (strstartswith(string, M3U_SAVEDISK))
      {
         /* Get volume name */
         char* disk_name = strright(string, strlen(string) - strlen(M3U_SAVEDISK));

         /* Add save disk, creating it if necessary */
         if (dc_add_m3u_save_disk(
               dc, m3u_file, save_dir,
               disk_name, save_disk_index))
            save_disk_index++;

         /* Clean up */
         if (disk_name != NULL)
            free(disk_name);
      }
      else if (!strstartswith(string, COMMENT))
      {
         /* Path format:
          *    FILE_NAME|FILE_LABEL
          * Delimiter + FILE_LABEL is optional */
         char file_name[RETRO_PATH_MAX]  = {0};
         char file_label[RETRO_PATH_MAX] = {0};
         char* delim_ptr                 = strchr(string, M3U_PATH_DELIM);
         bool label_set                  = false;

         if (delim_ptr)
         {
            /* Not going to use strleft()/strright() here,
             * since these functions allocate new strings,
             * which we don't want to do... */

            /* Get FILE_NAME segment */
            size_t len = (size_t)(1 + delim_ptr - string);
            if (len > 0)
               strncpy(file_name, string,
                     ((len < RETRO_PATH_MAX ? len : RETRO_PATH_MAX) * sizeof(char)) - 1);

            /* Get FILE_LABEL segment */
            delim_ptr++;
            if (*delim_ptr != '\0')
               strncpy(file_label, delim_ptr, sizeof(file_label) - 1);

            /* Note: If delimiter is present but FILE_LABEL
             * is omitted, label is intentionally left blank */
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

   /* If basedir was provided */
   if(basedir != NULL)
      free(basedir);

   /* Close the file */
   fclose(fp);
}

void dc_free(dc_storage* dc)
{
   /* Clean the struct */
   dc_reset(dc);
   free(dc);
   dc = NULL;
   return;
}

enum dc_image_type dc_get_image_type(const char* filename)
{
   /* Missing file */
   if (!filename || (*filename == '\0'))
      return DC_IMAGE_TYPE_NONE;

   /* Floppy image */
   if (strendswith(filename, "adf") ||
       strendswith(filename, "adz") ||
       strendswith(filename, "fdi") ||
       strendswith(filename, "dms") ||
       strendswith(filename, "ipf") ||
       strendswith(filename, "zip"))
      return DC_IMAGE_TYPE_FLOPPY;

   /* CD image */
   if (strendswith(filename, "cue") ||
       strendswith(filename, "ccd") ||
       strendswith(filename, "nrg") ||
       strendswith(filename, "mds") ||
       strendswith(filename, "iso"))
      return DC_IMAGE_TYPE_CD;

   /* HD image */
   if (strendswith(filename, "hdf") ||
       strendswith(filename, "hdz") ||
       path_is_directory(filename))
      return DC_IMAGE_TYPE_HD;

   /* WHDLoad */
   if (strendswith(filename, "lha") ||
       strendswith(filename, "slave") ||
       strendswith(filename, "info"))
      return DC_IMAGE_TYPE_WHDLOAD;

   /* Fallback */
   return DC_IMAGE_TYPE_UNKNOWN;
}
