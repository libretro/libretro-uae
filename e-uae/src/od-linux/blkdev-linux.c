 /*
  * E-UAE - The portable Amiga Emulator
  *
  * SCSI layer back-end for Linux hosts
  *
  * Copyright 2006 Jochen Becher
  * Copyright 2006 Richard Drummond
  *
  */

#ifdef SCSIEMU_LINUX_IOCTL

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "../include/memory.h"
#include "blkdev.h"
#include "scsidev.h"
#include "gui.h"

#include <linux/cdrom.h>
#include <sys/ioctl.h>

#define DEBUG_ME
#ifdef  DEBUG_ME
extern int log_scsi;
# define DEBUG_LOG    if (log_scsi) write_log
#else
# define DEBUG_LOG(...) do { ; } while (0);
#endif

struct scsidevdata {
    char *name;
    int fd;
    int isatapi;
    uae_u8 buf[DEVICE_SCSI_BUFSIZE];
};

#define MAX_SCSI_DRIVES 16
static struct scsidevdata drives[MAX_SCSI_DRIVES];
static int total_drives = 0;

static const uae_u8 *execscsicmd_in_ioctl (int unitnum, const uae_u8 *cmd_data, int cmd_len, int *outlen);

/*
 * this little piece of magic from Toni Wilen is needed to detect
 * ATAPI devices.
 */
static int is_atapi_drive (int unitnum)
{
    static const uae_u8 cmd[6] = {0x12, 0, 0, 0, 36, 0}; /* INQUIRY */
    uae_u8 out[36];
    int outlen = sizeof (out);
    const uae_u8 *p = execscsicmd_in_ioctl (unitnum, cmd, sizeof (cmd), &outlen);
    if (!p) {
	DEBUG_LOG ("SCSIDEV: Inquiry command failed; unit %d is not ATAPI drive\n", unitnum);
	return 0;
    }
    if (outlen >= 2 && (p[0] & 31) == 5 && (p[2] & 7) == 0) {
	DEBUG_LOG ("SCSIDEV: unit %d is ATAPI drive\n", unitnum);
	return 1;
    }
    DEBUG_LOG ("SCSIDEV: unit %d is not ATAPI drive\n", unitnum);
    return 0;
}

static int open_scsi_bus_ioctl (int flags)
{
    DEBUG_LOG ("SCSIDEV: open_scsi_bus_ioctl\n");
    /* TODO: scan everything / bus for drives; not just one drive */
    total_drives = 1;
    drives[0].name = currprefs.scsi_device;
    drives[0].fd = -1;
    drives[0].isatapi = 0;
    return 1;
}

static void close_scsi_bus_ioctl (void)
{
    DEBUG_LOG ("SCSIDEV: close_scsi_bus_ioctl\n");
}

static int open_scsi_device_ioctl (int unitnum)
{
    int result = 0;

    DEBUG_LOG ("SCSIDEV: unit = %d: open_scsi_device_ioctl\n", unitnum);
    if (unitnum < total_drives) {
	struct scsidevdata *sdd = &drives[unitnum];

	DEBUG_LOG ("SCSIDEV: unit = %d, name = %s, fd = %d\n", unitnum, sdd->name, sdd->fd);

	if (sdd->fd == -1) {
	    if ((sdd->fd = open (sdd->name, O_RDONLY|O_NONBLOCK)) != -1) {
		DEBUG_LOG ("SCSIDEV: Successfully opened drive %s\n", sdd->name);
		sdd->isatapi = is_atapi_drive (unitnum);
		result = 1;
	    } else {
		DEBUG_LOG ("SCSIDEV: Failed to open drive %s\n", sdd->name);
	    }
	} else {
	    /* already open */
	    DEBUG_LOG ("SCSIDEV: unit %d is already opened.\n", unitnum);
	    result = 1;
	}
    } else {
	DEBUG_LOG ("SCSIDEV: bad unit number %d\n", unitnum);
    }
    return result;
}

static void close_scsi_device_ioctl (int unitnum)
{
    struct scsidevdata *sdd;

    DEBUG_LOG ("SCSIDEV: unit = %d: close_scsi_device_ioctl\n", unitnum);
    if (unitnum >= total_drives) {
	DEBUG_LOG ("SCSIDEV: illegal unit %d >= total_drives %d.\n", unitnum, total_drives);
	return;
    }
    sdd = &drives[unitnum];
    if (sdd->fd != -1) {
	close (sdd->fd);
	sdd->fd = -1;
    }
}

static int media_check (struct scsidevdata *sdd)
{
    if (ioctl (sdd->fd, CDROM_DRIVE_STATUS, CDSL_CURRENT) == CDS_DISC_OK)
        return 1;
    else
        return 0;
}

static const uae_u8 *execscsicmd_out_ioctl (int unitnum, const uae_u8 *cmd_data, int cmd_len)
{
    struct scsidevdata *sdd;
    struct cdrom_generic_command cmd;
    int io_error;

    DEBUG_LOG ("SCSIDEV: unit = %d, execscsicmd_out_ioctl\n", unitnum);
    if (unitnum >= total_drives) {
	DEBUG_LOG ("SCSIDEV: illegal unit %d >= total_drives %d.\n", unitnum, total_drives);
	return 0;
    }
    sdd = &drives[unitnum];
    if (cmd_len > CDROM_PACKET_SIZE) {
	DEBUG_LOG ("SCSIDEV: cmd_len too large (%d)\n", cmd_len);
	return 0;
    }
    memcpy (cmd.cmd, cmd_data, cmd_len);
    cmd.buffer = 0;
    cmd.buflen = 0;
    cmd.stat = 0;
    cmd.sense = 0;
    cmd.data_direction = CGC_DATA_WRITE;
    cmd.quiet = 0;
    cmd.timeout = 80*60;

    gui_cd_led (1);

    io_error = ioctl (sdd->fd, CDROM_SEND_PACKET, &cmd);
    DEBUG_LOG ("SCSIDEV: error: %d, stat: %d\n", io_error, cmd.stat);
    if (io_error != 0) {
	DEBUG_LOG ("SCSIDEV: errno: %d, %s\n", errno, strerror (errno));
	return 0;
    }
    return cmd_data;
}

static const uae_u8 *execscsicmd_in_ioctl (int unitnum, const uae_u8 *cmd_data, int cmd_len, int *outlen)
{
    struct scsidevdata *sdd;
    struct cdrom_generic_command cmd;
    int io_error;

    DEBUG_LOG ("SCSIDEV: unit = %d, execscsicmd_in_ioctl\n", unitnum);
    if (unitnum >= total_drives) {
	DEBUG_LOG ("SCSIDEV: illegal unit %d >= total_drives %d.\n", unitnum, total_drives);
	return 0;
    }
    sdd = &drives[unitnum];
    if (cmd_len > CDROM_PACKET_SIZE) {
	DEBUG_LOG ("SCSIDEV: cmd_len too large (%d)\n", cmd_len);
	return 0;
    }
    memcpy (cmd.cmd, cmd_data, cmd_len);
    cmd.buffer = sdd->buf;
    cmd.buflen = sizeof (sdd->buf);
    cmd.stat = 0;
    cmd.sense = 0;
    cmd.data_direction = CGC_DATA_READ;
    cmd.quiet = 0;
    cmd.timeout = 80*60;

    gui_cd_led (1);

    io_error = ioctl (sdd->fd, CDROM_SEND_PACKET, &cmd);
    DEBUG_LOG ("SCSIDEV: error: %d, stat: %d\n", io_error, cmd.stat);
    if (io_error != 0) {
	DEBUG_LOG ("SCSIDEV: errno: %d, %s\n", errno, strerror (errno));
	return 0;
    }
    if (outlen) {
	*outlen = cmd.buflen;
    }
    return sdd->buf;
}

static int execscsicmd_direct_ioctl (int unitnum, uaecptr acmd)
{
    struct scsidevdata *sdd;
    struct cdrom_generic_command cmd;
    struct request_sense sense;

    uaecptr scsi_data         = get_long (acmd + 0);
    uae_u32 scsi_len          = get_long (acmd + 4);
    uaecptr scsi_cmd          = get_long (acmd + 12);
    int     scsi_cmd_len      = get_word (acmd + 16);
    uae_u8  scsi_flags        = get_byte (acmd + 20);
    uae_u8  scsi_status       = get_byte (acmd + 21);
    uaecptr scsi_sense        = get_long (acmd + 22);
    uae_u16 scsi_sense_len    = get_word (acmd + 26);

    int io_error;
    unsigned int senselen;
    int parm, i;

    addrbank *bank_data    = &get_mem_bank (scsi_data);
    addrbank *bank_cmd	   = &get_mem_bank (scsi_cmd);
    addrbank *bank_sense   = &get_mem_bank (scsi_sense);

    uae_u8   *scsi_datap;
    uae_u8   *scsi_datap_org;

    DEBUG_LOG ("SCSIDEV: unit = %d: execscsicmd_direct_ioctl\n", unitnum);
    DEBUG_LOG ("SCSIDEV: scsi_len = %d, scsi_cmd_len = %d, scsi_sense_len = %d, scsi_flags = %x\n",
	       scsi_len, scsi_cmd_len, scsi_sense_len, scsi_flags);

    if (unitnum >= total_drives) {
	DEBUG_LOG ("SCSIDEV: illegal unit %d >= total_drives %d.\n", unitnum, total_drives);
	return -1; /* TODO: better error code */
    }
    sdd = &drives[unitnum];

    /* do transfer directly to and from Amiga memory */
    if (!bank_data || !bank_data->check (scsi_data, scsi_len)) {
        DEBUG_LOG ("SCSIDEV: illegal Amiga memory buffer\n");
	return -5; /* IOERR_BADADDRESS */
    }

    if (scsi_cmd_len > CDROM_PACKET_SIZE) {
	DEBUG_LOG ("SCSIDEV: scsi_cmd_len too large (%d)\n", scsi_cmd_len);
	return -5; /* TODO: better code */
    }

    scsi_datap = scsi_datap_org = (scsi_len ? bank_data->xlateaddr (scsi_data) : 0);

    memcpy (cmd.cmd, bank_cmd->xlateaddr (scsi_cmd), scsi_cmd_len);
    cmd.buffer = scsi_datap;
    cmd.buflen = scsi_len;
    cmd.stat = scsi_status;
    if (sdd->isatapi) {
	scsi_atapi_fixup_pre (cmd.cmd, &scsi_cmd_len, &scsi_datap,
			      &scsi_len, &parm);
    }
    senselen = (scsi_flags & 4) ? 4 : /* SCSIF_OLDAUTOSENSE */
		      (scsi_flags & 2) ? scsi_sense_len : /* SCSIF_AUTOSENSE */
		      0;
    cmd.sense = senselen > 0 ? &sense : 0;
    cmd.data_direction = (scsi_flags & 1) ? CGC_DATA_READ : CGC_DATA_WRITE;
    cmd.quiet = 0;
    cmd.timeout = 80*60;

    gui_cd_led (1);

    io_error = ioctl (sdd->fd, CDROM_SEND_PACKET, &cmd);

    DEBUG_LOG ("SCSIDEV: error: %d, stat: %d\n", io_error, cmd.stat);

    if (cmd.stat != 0) {
        int n;

	io_error = 45;  /* HFERR_BadStatus */
	put_byte (acmd + 8, 0);
	put_byte (acmd + 18, 0 /*scsi_cmd_len */);
	put_byte (acmd + 21, cmd.stat);
	DEBUG_LOG ("SCSIDEV: bad status\n");
	n = cmd.sense ? cmd.sense->add_sense_len + 7 : 0;
	if (senselen > n) {
	    if (scsi_sense)
        	memset (bank_sense->xlateaddr (scsi_sense), 0, senselen);
	    senselen = n;
	}
	DEBUG_LOG ("SCSIDEV: senselen = %d\n", senselen);
	if (scsi_sense && cmd.sense && senselen > 0) {
	    memcpy (bank_sense->xlateaddr (scsi_sense), cmd.sense, senselen);
	}
	put_byte (acmd + 28, senselen);
    } else {
    	put_byte (acmd + 28, 0);
	if (scsi_sense && senselen > 0) {
	    memset (bank_sense->xlateaddr (scsi_sense), 0, senselen);
	}
	if (io_error == 0) {
	    if (sdd->isatapi) {
		scsi_atapi_fixup_post (cmd.cmd, scsi_cmd_len,
				       scsi_datap_org, scsi_datap,
				       &scsi_len, parm);
	    }
	    put_long (acmd + 8,  scsi_len);
	    put_word (acmd + 18, scsi_cmd_len);
	    put_byte (acmd + 21, cmd.stat);
	    io_error = 0;
	} else {
	    DEBUG_LOG ("SCSIDEV: errno: %d, %s\n", errno, strerror (errno));
	    put_long (acmd + 8,  0);
	    put_word (acmd + 18, 0);
	    put_byte (acmd + 21, cmd.stat);
	    io_error = 20; /* TODO: Map errors */
	}
    }
    if (scsi_datap != scsi_datap_org)
	free (scsi_datap);
    return io_error;
}

static struct device_info *info_device_ioctl (int unitnum, struct device_info *di)
{
    DEBUG_LOG ("SCSIDEV: unit = %d: info_device_ioctl\n", unitnum);

    if (unitnum < total_drives) {
	struct scsidevdata *sdd = &drives[unitnum];

	media_check (sdd);

	di->bus		    = 0;
	di->target	    = unitnum;
	di->lun		    = 0;
	di->media_inserted  = media_check (sdd);
	di->write_protected = 1;
	di->bytespersector  = 2048;
	di->cylinders	    = 1;
	di->type	    = INQ_ROMD; /* We only support CD/DVD drives for now */
	di->id		    = unitnum + 1;
	/* TODO: Create a more informative device label */
	sprintf (di->label, "[%s]", sdd->name);
    } else {
	di = 0;
    }

    return di;
}

static int pause_ioctl (int unitnum, int paused)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, pause_ioctl not implemented\n", unitnum);
    return 0;
}

static int stop_ioctl (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, stop_ioctl not implemented\n", unitnum);
    return 0;
}

static int play_ioctl (int unitnum, uae_u32 startmsf, uae_u32 endmsf, int scan)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, play_ioctl not implemented\n", unitnum);
    return 0;
}

static const uae_u8 *qcode_ioctl (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, qcode_ioctl not implemented\n", unitnum);
    return 0;
}

static const uae_u8 *toc_ioctl (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, toc_ioctl not implemented\n", unitnum);
    return 0;
}

static const uae_u8 *read_ioctl (int unitnum, int offset)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, read_ioctl not implemented\n", unitnum);
    return 0;
}

static int write_ioctl (int unitnum, int length, uae_u8 *buf)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, write_ioctl not implemented\n", unitnum);
    return 0;
}

static int check_isatapi_ioctl (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit = %d, check_isatapi_ioctl\n", unitnum);
    if (unitnum >= total_drives) {
	DEBUG_LOG ("SCSIDEV: illegal unit %d >= total_drives %d.\n", unitnum, total_drives);
	return 0;
    }
    return drives[unitnum].isatapi;
}

struct device_functions devicefunc_scsi_linux_ioctl = {
    open_scsi_bus_ioctl, /* open_bus_func     openbus; */
    close_scsi_bus_ioctl, /* close_bus_func    closebus; */
    open_scsi_device_ioctl, /* open_device_func  opendev; */
    close_scsi_device_ioctl, /* close_device_func closedev; */
    info_device_ioctl, /* info_device_func  info; */

    execscsicmd_out_ioctl, /* execscsicmd_out_func    exec_out; */
    execscsicmd_in_ioctl, /* execscsicmd_in_func     exec_in; */
    execscsicmd_direct_ioctl, /* execscsicmd_direct_func exec_direct; */

    pause_ioctl, /* pause_func pause; */
    stop_ioctl, /* stop_func  stop; */
    play_ioctl, /* play_func  play; */
    qcode_ioctl, /* qcode_func qcode; */
    toc_ioctl, /* toc_func   toc; */
    read_ioctl, /* read_func  read; */
    write_ioctl, /* write_func write; */

    check_isatapi_ioctl, /* isatapi_func isatapi; */

    0, /* open_device_func  opendevthread; */
    0 /* close_device_func closedevthread; */
};

#endif
