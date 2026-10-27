#ifndef LIBRETRO_WHDLOAD_H
#define LIBRETRO_WHDLOAD_H

#include <zlib.h>

/* Fingerprints identify PUAE-distributed binaries, never a user's scripts/prefs. */
static bool whdload_fingerprint(const char *path, size_t *size, uint32_t *checksum)
{
   unsigned char buf[4096];
   size_t n;
   bool ok;
   FILE *fp = fopen(path, "rb");
   if (!fp)
      return false;
   *size = 0;
   *checksum = crc32(0, NULL, 0);
   while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
   {
      *size += n;
      *checksum = crc32(*checksum, buf, n);
   }
   ok = !ferror(fp);
   if (fclose(fp))
      ok = false;
   return ok;
}

static bool whdload_old_binary(size_t size, uint32_t crc)
{
   static const struct { size_t size; uint32_t crc; } previous[] = {
      {161031, 0x9df148c1}, /* 20.0 */
      {157287, 0xf6e5b041}, /* 19.2 */
      {156867, 0x177db29e}, /* 19.1 */
      {151390, 0x5a408d08}, /* 18.9 */
      {151179, 0x18c34f5b}, /* 18.7 */
      {144978, 0x14cd5f0c}, /* 18.6 */
      {144902, 0x35f665a8}  /* 18.5 */
   };
   for (unsigned i = 0; i < sizeof(previous) / sizeof(previous[0]); i++)
      if (size == previous[i].size && crc == previous[i].crc)
         return true;
   return false;
}

/* Existing directories are not re-extracted wholesale: install missing payloads
 * and upgrade only recognized old PUAE binaries, with rollback on write errors. */
static void whdload_refresh_files(const char *helper)
{
   static const struct { const char *name; size_t size; uint32_t crc; } files[] = {
      {"C/WHDLoad", 162674, 0x1654051a},
      {"C/WHDLoad.VFS", 19132, 0x97ac5706},
      {"C/WHDLoad.VFS.020", 18936, 0x24ca983d},
      {"C/WArc", 19324, 0xad7a3ba6},
      {"Docs/WHDLoad-Copyright.html", 3286, 0x9733cf01}
   };
   bool update[sizeof(files) / sizeof(files[0])] = {false};
   bool needed = false;
   char stage[RETRO_PATH_MAX], archive[RETRO_PATH_MAX], payload[RETRO_PATH_MAX];
   size_t size;
   uint32_t crc;

   for (unsigned i = 0; i < sizeof(files) / sizeof(files[0]); i++)
   {
      char dst[RETRO_PATH_MAX];
      path_join(dst, helper, files[i].name);
      update[i] = !path_is_valid(dst);
      if (whdload_fingerprint(dst, &size, &crc))
      {
         if (size == files[i].size && crc == files[i].crc)
            continue;
         if (i == 0 && whdload_old_binary(size, crc))
            update[i] = true;
         else
            log_cb(RETRO_LOG_INFO, "WHDLoad: keeping user-supplied %s.\n", files[i].name);
      }
      needed |= update[i];
   }
   if (!needed)
      return;

   path_join(stage, retro_temp_directory, ".puae-whdload-20.1");
   remove_recurse(stage);
   if (!path_mkdir(stage))
      return;
   path_join(archive, stage, "helper.zip");
   path_join(payload, stage, "files");
   FILE *fp = fopen(archive, "wb");
   if (!fp)
      goto cleanup;
   bool written = fwrite(___whdload_WHDLoad_files_zip, 1,
         ___whdload_WHDLoad_files_zip_len, fp) == ___whdload_WHDLoad_files_zip_len;
   if (fclose(fp))
      written = false;
   if (!written || !path_mkdir(payload))
      goto cleanup;
   zip_uncompress(archive, payload, NULL);

   for (unsigned i = 0; i < sizeof(files) / sizeof(files[0]); i++)
   {
      char src[RETRO_PATH_MAX], dst[RETRO_PATH_MAX];
      char temporary[RETRO_PATH_MAX], backup[RETRO_PATH_MAX];
      size_t old_size = 0;
      uint32_t old_crc = 0;
      bool existed, moved = false;
      if (!update[i])
         continue;
      path_join(src, payload, files[i].name);
      path_join(dst, helper, files[i].name);
      if (!whdload_fingerprint(src, &size, &crc) || size != files[i].size || crc != files[i].crc)
         continue;
      snprintf(temporary, sizeof(temporary), "%s.puae-20.1-new", dst);
      snprintf(backup, sizeof(backup), "%s.pre-20.1", dst);
      /* Preserve unrelated files at the reserved temporary/backup names too. */
      if (path_is_valid(temporary) && (!whdload_fingerprint(temporary, &size, &crc)
            || size != files[i].size || crc != files[i].crc))
         continue;
      if (fcopy(src, temporary) < 0 || !whdload_fingerprint(temporary, &size, &crc)
            || size != files[i].size || crc != files[i].crc)
         goto file_error;
      existed = path_is_valid(dst);
      if (existed)
      {
         if (!whdload_fingerprint(dst, &old_size, &old_crc) || i != 0 || !whdload_old_binary(old_size, old_crc))
            goto file_error;
         if (path_is_valid(backup))
         {
            if (!whdload_fingerprint(backup, &size, &crc) || size != old_size || crc != old_crc || retro_remove(dst))
               goto file_error;
         }
         else if (rename(dst, backup))
            goto file_error;
         moved = true;
      }
      if (rename(temporary, dst))
      {
         if (moved && fcopy(backup, dst) < 0)
            log_cb(RETRO_LOG_ERROR, "WHDLoad: restore %s from %s.\n", dst, backup);
         goto file_error;
      }
      log_cb(RETRO_LOG_INFO, "WHDLoad 20.1: installed %s.\n", files[i].name);
      continue;
file_error:
      retro_remove(temporary);
      log_cb(RETRO_LOG_ERROR, "WHDLoad 20.1: unable to update %s.\n", files[i].name);
   }
cleanup:
   remove_recurse(stage);
}
#endif
