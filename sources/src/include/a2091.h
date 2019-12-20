#ifndef SRC_INCLUDE_A2091_H_INCLUDED
#define SRC_INCLUDE_A2091_H_INCLUDED 1

#include "filesys.h"

#ifdef A2091

#define WD33C93 _T("WD33C93")

extern addrbank dmaca2091_bank;
extern uae_u8 wdregs[32];
extern struct scsi_data *scsis[8];

void a2091_init (void);
void a2091_free (void);
void a2091_reset (void);

void a3000scsi_init (void);
void a3000scsi_free (void);
void a3000scsi_reset (void);
void rethink_a2091 (void);

void wdscsi_put (uae_u8);
uae_u8 wdscsi_get (void);
uae_u8 wdscsi_getauxstatus (void);
void wdscsi_sasr (uae_u8);

void scsi_hsync (void);

uae_u8 wdregs[32];
struct scsi_data *scsis[8];

#define WD33C93 _T("WD33C93")

int a2091_add_scsi_unit (int ch, struct uaedev_config_info *ci);
int a3000_add_scsi_unit (int ch, struct uaedev_config_info *ci);

int add_scsi_hd (int ch, struct hd_hardfiledata *hfd, struct uaedev_config_info *ci, int scsi_level);
int add_scsi_cd (int ch, int unitnum);

#endif // A2091

#endif
