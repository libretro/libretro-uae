/*
 * UAE - The Un*x Amiga Emulator
 *
 * Block device access using libscg
 *
 * Copyright 2004-2005 Richard Drummond
 *
 * Heavily based on code:
 * Copyright 1995 Bernd Schmidt
 * Copyright 1999 Patrick Ohly
 *
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "memory_uae.h"
#include "threaddep/thread.h"
#include "blkdev.h"
#include "scsidev.h"
#include "sleep.h"
#include "gui.h"

//#define DEBUG_ME
#ifdef  DEBUG_ME
#define DEBUG_LOG    write_log
#else
#define DEBUG_LOG(...) { }
#endif

typedef int BOOL;

#include "scg/scgcmd.h"
#include "scg/scsidefs.h"
#include "scg/scsireg.h"
#include "scg/scsitransp.h"

#define MAX_DRIVES	16

struct scsidevdata {
    int   bus, target, lun;
    int   isatapi;
    int   max_dma;
    SCSI *scgp;
};

static struct scsidevdata drives[MAX_DRIVES];
static int total_drives;

static const uae_u8 *execscsicmd_in (int unitnum, const uae_u8 *data, int len, int *outlen);

/*
 * scg_isatapi() is not implemented on all platforms. Therefore,
 * this little piece of magic from Toni Wilen is needed to detect
 * ATAPI devices.
 */
static int is_atapi_drive (int unitnum)
{
     static const uae_u8 cmd[6] = {0x12, 0, 0, 0, 36, 0}; /* INQUIRY */
     uae_u8 out[36];
     int outlen = sizeof (out);
     const uae_u8 *p = execscsicmd_in (unitnum, cmd, sizeof (cmd), &outlen);
     if (!p)
	return 0;
     if (outlen >= 2 && (p[0] & 31) == 5 && (p[2] & 7) == 0)
	return 1;
     return 0;
}

static int add_drive (SCSI *scgp)
{
   int result = 0;

   if (total_drives < MAX_DRIVES) {
	int isatapi;

	drives [total_drives].bus     = scgp->addr.scsibus;
	drives [total_drives].target  = scgp->addr.target;
	drives [total_drives].lun     = scgp->addr.lun;

	isatapi = scg_isatapi (scgp);

	/* If scg_isatapi returned false, we need to double-check
	 * because it may not be implemented on the target platform
	 */
	if (!isatapi) {
	    drives [total_drives].scgp = scgp;
	    isatapi = is_atapi_drive (total_drives);
	    drives [total_drives].scgp = 0;
	}
	drives [total_drives].isatapi = isatapi;

	total_drives++;
	result = 1;
    }

    return result;
}

/* Global lock - this needs to be replaced with a per-device lock */
uae_sem_t scgp_sem;

/********** generic SCSI stuff stolen from cdrecord and scsitransp.c *********/

static int inquiry (SCSI *scgp, void *bp, int cnt)
{
    struct scg_cmd *scmd = scgp->scmd;
    int result;

    memset (bp, 0, cnt);
    memset ((void *)scmd, 0, sizeof(*scmd));
    scmd->addr      = bp;
    scmd->size      = cnt;
    scmd->flags     = SCG_RECV_DATA|SCG_DISRE_ENA;
    scmd->cdb_len   = SC_G0_CDBLEN;
    scmd->sense_len = CCS_SENSE_LEN;
    scmd->cdb.g0_cdb.cmd   = SC_INQUIRY;
    scmd->cdb.g0_cdb.lun   = scg_lun(scgp);
    scmd->cdb.g0_cdb.count = cnt;

    scgp->cmdname = "inquiry";

    result = scg_cmd(scgp);

    return result;
}

static int test_unit_ready (SCSI *scgp)
{
    struct scg_cmd *scmd = scgp->scmd;
    int result;

    memset ((void *)scmd, 0, sizeof(*scmd));
    scmd->addr      = (caddr_t)0;
    scmd->size      = 0;
    scmd->flags     = SCG_DISRE_ENA | (scgp->silent ? SCG_SILENT:0);
    scmd->cdb_len   = SC_G0_CDBLEN;
    scmd->sense_len = CCS_SENSE_LEN;
    scmd->cdb.g0_cdb.cmd = SC_TEST_UNIT_READY;
    scmd->cdb.g0_cdb.lun = scg_lun(scgp);

    scgp->cmdname = "test unit ready";

    result = scg_cmd (scgp);

    return result;
}

static int unit_ready (SCSI *scgp)
{
    register struct scg_cmd *scmd = scgp->scmd;

    if (test_unit_ready(scgp) >= 0)             /* alles OK */
	return 1;
    else if (scmd->error >= SCG_FATAL)          /* nicht selektierbar */
	return 0;

    if (scg_sense_key(scgp) == SC_UNIT_ATTENTION) {
	if (test_unit_ready(scgp) >= 0)         /* alles OK */
	     return 1;
    }
    if ((scg_cmd_status(scgp) & ST_BUSY) != 0) {
	/* Busy/reservation_conflict */
	uae_msleep (500);

	if (test_unit_ready(scgp) >= 0)         /* alles OK */
	    return 1;
    }
    if (scg_sense_key(scgp) == -1) {            /* non extended Sense */
	if (scg_sense_code(scgp) == 4)          /* NOT_READY */
	    return 0;
	return 1;
    }
						/* FALSE wenn NOT_READY */
    return (scg_sense_key (scgp) != SC_NOT_READY);
}

static void print_product (const struct scsi_inquiry *ip)
{
    write_log ("'%.8s' ",  ip->vendor_info);
    write_log ("'%.16s' ", ip->prod_ident);
    write_log ("'%.4s' ",  ip->prod_revision);

    if (ip->type == INQ_ROMD)
	write_log ("CD-ROM");
    else if (ip->type == INQ_DASD)
	write_log ("Disk");
}

/* get integer value from env or return default value, if unset */
static int getenvint (const char *varname, int def)
{
    const char *val = getenv (varname);
    return val ? atoi (val) : def;
}

static SCSI *openscsi (int scsibus, int target, int lun)
{
    SCSI *scgp = scg_smalloc ();

    if (scgp) {
	char *device;

	scgp->debug   = getenvint ("UAE_SCSI_DEBUG", 0);
	scgp->kdebug  = getenvint ("UAE_SCSI_KDEBUG", 0);
	scgp->silent  = getenvint ("UAE_SCSI_SILENT", 1);
	scgp->verbose = getenvint ("UAE_SCSI_VERBOSE", 0);
	device        = getenv    ("UAE_SCSI_DEVICE");

	if (!device || (strlen(device) == 0))
	    device = currprefs.scsi_device;

	write_log ("SCSIDEV: Device '%s'\n", device);

	scg_settarget (scgp, scsibus, target, lun);
	scg_settimeout (scgp, 80*60);

	if (scg__open (scgp, device) <= 0) {
	    if (scgp->errstr)
		write_log ("SCSIDEV: Failed to open '%s': %s\n",
			   device, scgp->errstr);
	    scg_sfree (scgp);
	    scgp = 0;
       }
    }
    return scgp;
}

static void closescsi (SCSI *scgp)
{
    scg__close (scgp);
    scg_sfree (scgp);
}

/********************* start of our own code ************************/

static uae_u8 scsibuf [DEVICE_SCSI_BUFSIZE];

static int execscsicmd (int unitnum, const uae_u8 *data, int len, uae_u8 *inbuf,
			int inlen)
{
    int sactual = 0;
    struct scsidevdata *sdd = &drives[unitnum];
    SCSI *scgp              = sdd->scgp;
    struct scg_cmd *scmd;

    scmd = scgp->scmd;

    DEBUG_LOG ("SCSIDEV: execscicmd data=%08lx len=%d, inbuf=%08lx"\
	       " inlen=%d\n", data, len, inbuf, inlen);

    uae_sem_wait (&scgp_sem);
    memset (scmd, 0, sizeof (*scmd));

    scmd->timeout   = 80 * 60;
    if (inbuf) {
	scmd->addr      = (caddr_t) inbuf;
	scmd->size      = inlen;
	scmd->flags     = SCG_RECV_DATA;
	memset (inbuf, 0, inlen);
    } else {
	scmd->flags     = SCG_DISRE_ENA;
    }

    scmd->cdb_len   = len;
    memcpy (&scmd->cdb, data, len);
    scmd->target    = sdd->target;
    scmd->sense_len = -1;
    scmd->sense_count = 0;
    *(uae_u8 *)&scmd->scb = 0;

    scg_settarget (scgp, sdd->bus, sdd->target, sdd->lun);
    scgp->cmdname    = "???";
    scgp->curcmdname = "???";

    DEBUG_LOG ("SCSIDEV: sending command: 0x%2x\n", scmd->cdb.g0_cdb.cmd);

	gui_flicker_led (LED_CD, 0, 1);

    scg_cmd (scgp);

    uae_sem_post (&scgp_sem);

    DEBUG_LOG ("SCSIDEV: result: %d %d\n", scmd->error, scmd->ux_errno);

    return scmd->size;
}

static int execscsicmd_direct (int unitnum, uaecptr acmd)
{
    int sactual = 0;
    struct scsidevdata *sdd = &drives[unitnum];
    SCSI *scgp              = sdd->scgp;
    struct scg_cmd *scmd    = scgp->scmd;

    uaecptr scsi_data      = get_long (acmd + 0);
    uae_u32 scsi_len       = get_long (acmd + 4);
    uaecptr scsi_cmd       = get_long (acmd + 12);
    int scsi_cmd_len       = get_word (acmd + 16);
    int scsi_cmd_len_orig  = scsi_cmd_len;
    uae_u8  scsi_flags     = get_byte (acmd + 20);
    uaecptr scsi_sense     = get_long (acmd + 22);
    uae_u16 scsi_sense_len = get_word (acmd + 26);
    int io_error           = 0;
    int parm;
    addrbank *bank_data    = &get_mem_bank (scsi_data);
    addrbank *bank_cmd	   = &get_mem_bank (scsi_cmd);
    uae_u8   *scsi_datap;
    uae_u8   *scsi_datap_org;

    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_direct\n", unitnum);

    /* do transfer directly to and from Amiga memory */
    if (!bank_data || !bank_data->check (scsi_data, scsi_len))
	return -5; /* IOERR_BADADDRESS */

    uae_sem_wait (&scgp_sem);

    memset (scmd, 0, sizeof (*scmd));
    /* the Amiga does not tell us how long the timeout shall be, so make it
     * _very_ long (specified in seconds) */
    scmd->timeout   = 80 * 60;
    scsi_datap      = scsi_datap_org = scsi_len
		      ? bank_data->xlateaddr (scsi_data) : 0;
    scmd->size      = scsi_len;
    scmd->flags     = (scsi_flags & 1) ? SCG_RECV_DATA : SCG_DISRE_ENA;
    memcpy (&scmd->cdb, bank_cmd->xlateaddr (scsi_cmd), scsi_cmd_len);
    scmd->target    = sdd->target;
    scmd->sense_len = (scsi_flags & 4) ? 4 : /* SCSIF_OLDAUTOSENSE */
		      (scsi_flags & 2) ? scsi_sense_len : /* SCSIF_AUTOSENSE */
		      -1;
    scmd->sense_count = 0;
    *(uae_u8 *)&scmd->scb = 0;
    if (sdd->isatapi)
	scsi_atapi_fixup_pre (scmd->cdb.cmd_cdb, &scsi_cmd_len, &scsi_datap,
			      &scsi_len, &parm);
    scmd->addr      = (caddr_t)scsi_datap;
    scmd->cdb_len   = scsi_cmd_len;

    scg_settarget (scgp, sdd->bus, sdd->target, sdd->lun);
    scgp->cmdname    = "???";
    scgp->curcmdname = "???";

    DEBUG_LOG ("SCSIDEV: sending command: 0x%2x\n", scmd->cdb.g0_cdb.cmd);

    scg_cmd (scgp);

    DEBUG_LOG ("SCSIDEV: result: %d %d %s\n", scmd->error, scmd->ux_errno,\
	       scgp->errstr);

	gui_flicker_led (LED_CD, 0, 1);

    put_word (acmd + 18, scmd->error == SCG_FATAL
					? 0 : scsi_cmd_len); /* fake scsi_CmdActual */
    put_byte (acmd + 21, *(uae_u8 *)&scmd->scb);	     /* scsi_Status */

    if (*(uae_u8 *)&scmd->scb) {
	io_error = 45; /* HFERR_BadStatus */
	/* copy sense? */
	for (sactual = 0;
	     scsi_sense && sactual < scsi_sense_len && sactual < scmd->sense_count;
	     sactual++) {
	     put_byte (scsi_sense + sactual, scmd->u_sense.cmd_sense[sactual]);
	}
	put_long (acmd + 8, 0); /* scsi_Actual */
    } else {
	int i;
	for (i = 0; i < scsi_sense_len; i++)
	    put_byte (scsi_sense + i, 0);
	sactual = 0;
	if (scmd->error != SCG_NO_ERROR || scmd->ux_errno != 0) {
	    /* We might have been limited by the hosts DMA limits,
	       which is usually indicated by ENOMEM */
	    if (scsi_len > (unsigned int)sdd->max_dma && scmd->ux_errno == ENOMEM)
		io_error = (uae_u8)-4; /* IOERR_BADLENGTH */
	    else {
		io_error = 20; /* io_Error, but not specified */
		put_long (acmd + 8, 0); /* scsi_Actual */
	    }
	} else {
	    scsi_len = scmd->size;
	    if (sdd->isatapi)
		scsi_atapi_fixup_post (scmd->cdb.cmd_cdb, scsi_cmd_len,
				       scsi_datap_org, scsi_datap,
				       &scsi_len, parm);
	    io_error = 0;
	    put_long (acmd + 8, scsi_len); /* scsi_Actual */
	}
    }
    put_word (acmd + 28, sactual);

    uae_sem_post (&scgp_sem);

    if (scsi_datap != scsi_datap_org)
	xfree (scsi_datap);

    return io_error;
}

static const uae_u8 *execscsicmd_out (int unitnum, const uae_u8 *data, int len)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_out\n", unitnum);

    if (execscsicmd (unitnum, data, len, 0, 0) < 0)
	return 0;

    return data;
}

static const uae_u8 *execscsicmd_in (int unitnum, const uae_u8 *data, int len, int *outlen)
{
    int v;

    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_in\n", unitnum);

    v = execscsicmd (unitnum, data, len, scsibuf, DEVICE_SCSI_BUFSIZE);

    if (v < 0)
	return 0;
    if (v == 0)
	return 0;
    if (outlen)
	*outlen = v;

    return scsibuf;
}

/*
 * Scan SCSI busses to detect any devices
 * that we want to supply to the Amgia.
 *
 * Based on code from cdrecord
 */
static int scanscsi (SCSI *scgp)
{
    int bus;
    int tgt;
    int lun = 0;
    int initiator;
    int have_tgt;
    int n;

    scgp->silent++;
    for (bus = 0; bus < 16; bus++) {
	scg_settarget (scgp, bus, 0, 0);

	if (!scg_havebus (scgp, bus))
	    continue;

	initiator = scg_initiator_id (scgp);
	write_log ("scsibus%d:\n", bus);

	for (tgt = 0; tgt < 16; tgt++) {
	    n = bus * 100 + tgt;

	    scg_settarget (scgp, bus, tgt, lun);
	    have_tgt = unit_ready (scgp) || scgp->scmd->error != SCG_FATAL;

	    if (!have_tgt && tgt > 7) {
		if (scgp->scmd->ux_errno == EINVAL)
		    break;
		continue;
	    }

	    write_log ("  %d,%d,%d  %d ", bus, tgt, lun, n);

	    if (tgt == initiator) {
		write_log ("HOST ADAPTOR\n");
		continue;
	    }
	    if (!have_tgt) {
		/* Hack: fd -> -2 means no access */
		write_log ( "%c\n", scgp->fd == -2 ? '?':'*');
		continue;
	    }

	    if ((scgp->scmd->error < SCG_FATAL)
		|| (scgp->scmd->scb.chk && scgp->scmd->sense_count > 0)) {
		struct scsi_inquiry *inq = scgp->inq;

		inquiry (scgp, inq, sizeof (*inq));
		print_product (inq);

		/* Just CD/DVD drives for now */
		if (inq->type == INQ_ROMD)
		    add_drive (scgp);
	    }

	    write_log ("\n");
	}
    }
    scgp->silent--;
    return 0;
}

static int open_scsi_bus (int flags)
{
    int   result = 0;
    int   debug, verbose;
    SCSI *scgp_scan;
    char *device;
    char  errstr[128];
    static int init = 0;

    DEBUG_LOG ("SCSIDEV: open_scsi_bus\n");

    if (!init) {
	init = 1;
	uae_sem_init (&scgp_sem, 0, 1);
	/* TODO: replace global lock with per-device locks */
    }

    debug   = getenvint ("UAE_SCSI_DEBUG", 0);
    verbose = getenvint ("UAE_SCSI_VERBOSE", 0);
    device  = getenv    ("UAE_SCSI_DEVICE");

    if (!device || (strlen (device) == 0))
	device = currprefs.scsi_device;

    if ((scgp_scan = scg_open (device, errstr, sizeof (errstr),
					       debug, verbose)) != (SCSI *)0) {
	scanscsi (scgp_scan);
	result = 1;
	scg_close (scgp_scan);
    } else {
	write_log ("SCSIDEV: can't open bus: %s\n", errstr);
    }

    write_log ("SCSIDEV: %d devices found\n", total_drives);
    return result;
}

static void close_scsi_bus (void)
{
    int i;

    DEBUG_LOG ("SCSIDEV: close_scsi_bus\n");

    for (i = 0; i < total_drives; i++) {
	closescsi (drives[i].scgp);
	drives[i].scgp = 0;
    }
}

static int open_scsi_device (int unitnum)
{
    int result = 0;

    DEBUG_LOG ("SCSIDEV: unit=%d: open_scsi_device\n", unitnum);

    if (unitnum < total_drives) {
	struct scsidevdata *sdd = &drives[unitnum];

	if (!sdd->scgp) {
	    if ((sdd->scgp = openscsi (sdd->bus, sdd->target, sdd->lun)) != 0)
		result = 1;
	} else
	    /* already open */
	    result = 1;
    }
    return result;
}

static void close_scsi_device (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: close_scsi_device\n", unitnum);

    /* do nothing - leave devices open until the bus is closed */
}

static int media_check (SCSI *scgp)
{
    int media = 0;

    uae_sem_wait (&scgp_sem);

    if (test_unit_ready (scgp) >= 0)
	media = 1;

    uae_sem_post (&scgp_sem);

    DEBUG_LOG ("SCSIDEV: media check :%d\n", media);

    return media;
}

static struct device_info *info_device (int unitnum, struct device_info *di)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: info_device\n", unitnum);

    if (unitnum < total_drives) {
		struct scsidevdata *sdd = &drives[unitnum];

		di->bus				= 0;
		di->target			= unitnum;
		di->lun				= 0;
		di->media_inserted	= media_check (sdd->scgp);
		di->write_protected	= 1;
		di->bytespersector	= 2048;
		di->cylinders		= 1;
		di->type			= INQ_ROMD; /* We only support CD/DVD drives for now */
		di->unitnum			= unitnum + 1;

		/* TODO: Create a more informative device label */
		sprintf (di->label, "(%d,%d,%d)", sdd->bus, sdd->target, sdd->lun);
    } else
		di = 0;

	return di;
}

static int check_isatapi (int unitnum)
{
    return drives[unitnum].isatapi;
}

struct device_functions devicefunc_scsi_libscg = {
    open_scsi_bus,
    close_scsi_bus,
    open_scsi_device,
    close_scsi_device,
    info_device,
    execscsicmd_out,
    execscsicmd_in,
    execscsicmd_direct,
    0, 0, 0, 0, 0, 0, 0,
    check_isatapi,
    0, 0
};
