#pragma once
#ifndef SRC_INCLUDE_CRC32_H_INCLUDED
#define SRC_INCLUDE_CRC32_H_INCLUDED 1

extern uae_u32 get_crc32 (uae_u8 *buf, int len);
extern uae_u16 get_crc16 (uae_u8 *buf, int len);
extern uae_u32 get_crc32_val (uae_u8 v, uae_u32 crc);
extern void get_sha1 (uae_u8 *input, int len, uae_u8 *out);
extern const TCHAR *get_sha1_txt (uae_u8 *input, int len);
#define SHA1_SIZE 20

#endif // SRC_INCLUDE_CRC32_H_INCLUDED
