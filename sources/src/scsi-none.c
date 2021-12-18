#include "sysconfig.h"
#include "sysdeps.h"

#include "scsidev.h"

uaecptr scsidev_startup (uaecptr resaddr) { return resaddr; }
void scsidev_install (void) {}
void scsidev_reset (void) {}
void scsidev_start_threads (void) {}

int scsi_do_disk_change (int unitnum, int insert, int *pollmode) { return -1; }
