#pragma once
#ifndef SRC_INCLUDE_CIA_H_INCLUDED
#define SRC_INCLUDE_CIA_H_INCLUDED 1

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * CIA chip support
  *
  * (c) 1995 Bernd Schmidt
  */

void CIA_reset (void);
void CIA_vsync_prehandler (void);
void CIA_hsync_prehandler (void);
void CIA_vsync_posthandler (bool);
void CIA_hsync_posthandler (bool);
void CIA_handler (void);

void cia_heartbeat (void);

void diskindex_handler (void);
void cia_parallelack (void);
void cia_diskindex (void);

void dumpcia (void);
void rethink_cias (void);
int resetwarning_do (int);
void cia_set_overlay (bool);

int parallel_direct_write_data (uae_u8, uae_u8);
int parallel_direct_read_data (uae_u8*);
int parallel_direct_write_status (uae_u8, uae_u8);
int parallel_direct_read_status (uae_u8*);

void rtc_hardreset (void);

#endif // SRC_INCLUDE_CIA_H_INCLUDED
