#pragma once
#ifndef SRC_INCLUDE_CFGFILE_H_INCLUDED
#define SRC_INCLUDE_CFGFILE_H_INCLUDED 1

#include "options.h"
#include "zfile.h"

int cfgfile_yesno (const TCHAR *option, const TCHAR *value, const TCHAR *name, bool *location);
int cfgfile_intval (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location, int scale);
int cfgfile_strval (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location, const TCHAR *table[], int more);
int cfgfile_string (const TCHAR *option, const TCHAR *value, const TCHAR *name, TCHAR *location, int maxsz);
TCHAR *cfgfile_subst_path (const TCHAR *path, const TCHAR *subst, const TCHAR *file);
int cfgfile_load (struct uae_prefs *p, const TCHAR *filename, int *type, int ignorelink, int userconfig);
int cfgfile_save (struct uae_prefs *p, const TCHAR *filename, int);
void cfgfile_parse_line (struct uae_prefs *p, TCHAR *, int);
void cfgfile_parse_lines (struct uae_prefs *p, const TCHAR *, int);
int cfgfile_parse_option (struct uae_prefs *p, TCHAR *option, TCHAR *value, int);
int cfgfile_get_description (const TCHAR *filename, TCHAR *description, TCHAR *hostlink, TCHAR *hardwarelink, int *type);
void cfgfile_show_usage (void);
uae_u32 cfgfile_uaelib (int mode, uae_u32 name, uae_u32 dst, uae_u32 maxlen);
uae_u32 cfgfile_uaelib_modify (uae_u32 mode, uae_u32 parms, uae_u32 size, uae_u32 out, uae_u32 outsize);
uae_u32 cfgfile_modify (uae_u32 index, TCHAR *parms, uae_u32 size, TCHAR *out, uae_u32 outsize);
void cfgfile_addcfgparam (TCHAR *);
int built_in_prefs (struct uae_prefs *p, int model, int config, int compa, int romcheck);
int built_in_chipset_prefs (struct uae_prefs *p);
int cmdlineparser (const TCHAR *s, TCHAR *outp[], int max);
int cfgfile_configuration_change (int);

void cfgfile_dwrite (struct zfile *, const TCHAR *option, const TCHAR *format,...);
void cfgfile_target_write (struct zfile *, const TCHAR *option, const TCHAR *format,...);
void cfgfile_target_dwrite (struct zfile *, const TCHAR *option, const TCHAR *format,...);
void cfgfile_write_bool (struct zfile *f, const TCHAR *option, bool b);
void cfgfile_dwrite_bool (struct zfile *f,const  TCHAR *option, bool b);
void cfgfile_target_write_bool (struct zfile *f, const TCHAR *option, bool b);
void cfgfile_target_dwrite_bool (struct zfile *f, const TCHAR *option, bool b);
void cfgfile_write_str (struct zfile *f, const TCHAR *option, const TCHAR *value);
void cfgfile_dwrite_str (struct zfile *f, const TCHAR *option, const TCHAR *value);
void cfgfile_target_write_str (struct zfile *f, const TCHAR *option, const TCHAR *value);
void cfgfile_target_dwrite_str (struct zfile *f, const TCHAR *option, const TCHAR *value);
void cfgfile_save_options (struct zfile *f, struct uae_prefs *p, int type);
int cfgfile_yesno2 (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location);
int cfgfile_doubleval (const TCHAR *option, const TCHAR *value, const TCHAR *name, double *location);
int cfgfile_intval_unsigned (const TCHAR *option, const TCHAR *value, const TCHAR *name, unsigned int *location, int scale);
int cfgfile_strboolval (const TCHAR *option, const TCHAR *value, const TCHAR *name, bool *location, const TCHAR *table[], int more);
int cfgfile_path_mp (const TCHAR *option, const TCHAR *value, const TCHAR *name, TCHAR *location, int maxsz, struct multipath *mp);
int cfgfile_path (const TCHAR *option, const TCHAR *value, const TCHAR *name, TCHAR *location, int maxsz);
int cfgfile_multipath (const TCHAR *option, const TCHAR *value, const TCHAR *name, struct multipath *mp);
int cfgfile_rom (const TCHAR *option, const TCHAR *value, const TCHAR *name, TCHAR *location, int maxsz);


#endif // SRC_INCLUDE_CFGFILE_H_INCLUDED

