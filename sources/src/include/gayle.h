#pragma once
#ifndef SRC_INCLUDE_GAYLE_H_INCLUDED
#define SRC_INCLUDE_GAYLE_H_INCLUDED 1

#ifdef GAYLE
void gayle_reset (int);
void gayle_hsync (void);
void gayle_free (void);
int gayle_add_ide_unit (int ch, struct uaedev_config_info *ci);
int gayle_modify_pcmcia_sram_unit (const TCHAR *path, int readonly, int insert);
int gayle_modify_pcmcia_ide_unit (const TCHAR *path, int readonly, int insert);
int gayle_add_pcmcia_sram_unit (const TCHAR *path, int readonly);
int gayle_add_pcmcia_ide_unit (const TCHAR *path, int readonly);
void gayle_free_units (void);
void rethink_gayle (void);
void gayle_map_pcmcia (void);
#endif // GAYLE

#define PCMCIA_COMMON_START 0x600000
#define PCMCIA_COMMON_SIZE 0x400000

#endif // SRC_INCLUDE_GAYLE_H_INCLUDED
