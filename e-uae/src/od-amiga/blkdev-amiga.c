 /*
  * E-UAE - The portable Amiga Emulator
  *
  * SCSI layer back-end for AmigaOS hosts
  *
  * Copyright 2005-2006 Richard Drummond
  *           2005 Sigbjørn Skjæret (68k/MorphOS device-scanning)
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "include/memory.h"
#include "threaddep/thread.h"
#include "blkdev.h"
#include "scsidev.h"
#include "sleep.h"
#include "gui.h"

#include <devices/scsidisk.h>
#include <proto/exec.h>


//#define DEBUG_ME
#ifdef  DEBUG_ME
#define DEBUG_LOG    write_log
#else
#define DEBUG_LOG(...) do { ; } while (0);
#endif

#define MAX_DRIVES	16


struct IOContext {
    struct MsgPort  *msgport;
    struct IOStdReq *ioreq;
    struct SCSICmd   cmd;
    uae_u8          *scsibuf;
};

/*
 * Private per-device data
 */
struct scsidevdata {
    const char       *device;
    int               unit;
/*
 * Aargh! AmigaOS device I/O requires the task that opened the device
 * be the task that sends I/O requests. Thus, since we want to be able
 * to access each device from two tasks, we need to maintain two I/O
 * contexts - one for each task.
 *
 * TODO: Rework the upper layers of the SCSI wrapper so we can get
 * rid of this.
 */
    struct Task      *main_task;
    struct IOContext  main_ioc;

    struct Task      *local_task;
    struct IOContext  local_ioc;
};

static struct scsidevdata drives[MAX_DRIVES];
static int total_drives;


static int open_iocontext (const char *device, int unit, struct IOContext *ioc)
{
    int error = -1;

    DEBUG_LOG ("SCSIDEV: Opening device '%s' unit %d...", device, unit);

    if ((ioc->msgport = CreateMsgPort ())) {
	if ((ioc->ioreq = (struct IOStdReq*) CreateIORequest (ioc->msgport, sizeof(struct IOStdReq)))) {
	    error = OpenDevice ((char *)device,
				unit,
				(struct IORequest*)ioc->ioreq,
				0);
	    if (error == 0) {
	        ioc->scsibuf = AllocMem (DEVICE_SCSI_BUFSIZE, MEMF_ANY);

		if (ioc->scsibuf) {
		    DEBUG_LOG ("Success\n");

		    return 0;
		}
		CloseDevice ((struct IORequest *)ioc->ioreq);
	    }
	    DeleteIORequest ((struct IORequest *)ioc->ioreq);
	    ioc->ioreq = 0;
	}
	DeleteMsgPort (ioc->msgport);
	ioc->msgport = 0;
    }

    DEBUG_LOG ("Failed. Error=%d\n", error);

    return error;
}

static void close_iocontext (struct IOContext *ioc)
{
    if (ioc->scsibuf) {
	FreeMem (ioc->scsibuf, DEVICE_SCSI_BUFSIZE);
	ioc->scsibuf = 0;
    }

    if (ioc->ioreq) {
	CloseDevice ((struct IORequest *)ioc->ioreq);
	DeleteIORequest ((struct IORequest *)ioc->ioreq);
	ioc->ioreq = 0;
    }

    if (ioc->msgport) {
	DeleteMsgPort (ioc->msgport);
	ioc->msgport = 0;
    }
}

static struct IOContext *get_iocontext (int unitnum)
{
    struct scsidevdata *sdd  = &drives[unitnum];
    struct Task        *self = FindTask (NULL);
    struct IOContext   *ioc  = NULL;

    if (self == sdd->main_task) {
	DEBUG_LOG ("SCSIDEV: Doing I/O in main task\n");

	ioc = &(sdd->main_ioc);
    } else if (self == sdd->local_task) {
	DEBUG_LOG ("SCSIDEV: Doing I/O in local task\n");
	ioc = &(sdd->local_ioc);
    }

    return ioc;
}


static int add_device (const char *device, int unit)
{
   int result = 0;

   if (total_drives < MAX_DRIVES) {
        struct scsidevdata *sdd = &drives[total_drives];

	memset (sdd, 0, sizeof (struct scsidevdata));

	sdd->device    = my_strdup (device); // FIXME: This will leak.
	sdd->unit      = unit;
	sdd->main_task = FindTask (NULL);

	total_drives++;
	result = 1;
    }
    return result;
}

static int execscsicmd (struct IOContext *ioc, const uae_u8 *cmd_data, int cmd_len,
			uae_u8 *inbuf, int inlen, int *outlen)
{
    struct IOStdReq  *ioreq = ioc->ioreq;
    struct SCSICmd   *cmd   = &(ioc->cmd);
    int error;

    if (ioc->ioreq == 0)
	return -1;

    memset (cmd, 0, sizeof (struct SCSICmd));

    ioreq->io_Length  = sizeof(struct SCSICmd);
    ioreq->io_Data    = (APTR)cmd;
    ioreq->io_Command = HD_SCSICMD;

    DEBUG_LOG ("SCSIDEV: execscicmd data=%08lx len=%d, inbuf=%08lx"\
	       " inlen=%d\n", cmd_data, cmd_len, inbuf, inlen);

    if (inbuf) {
	cmd->scsi_Data   = (UWORD*)inbuf;
	cmd->scsi_Length = inlen;
	cmd->scsi_Flags  = SCSIF_READ | SCSIF_AUTOSENSE;
	memset (inbuf, 0, inlen);
    } else
	cmd->scsi_Flags  = SCSIF_WRITE | SCSIF_AUTOSENSE;

    cmd->scsi_Command   = (UBYTE*)cmd_data;
    cmd->scsi_CmdLength = cmd_len;

    DEBUG_LOG ("SCSIDEV: sending command: 0x%2x\n", cmd->scsi_Command[0]);

    gui_cd_led (1);

    error = DoIO ((struct IORequest *)ioreq);

    DEBUG_LOG ("SCSIDEV: result: %d\n", error);
    DEBUG_LOG ("SCSIDEV: actual: %d\n", cmd->scsi_Actual);

    if (outlen)
	*outlen = cmd->scsi_Actual;

    return error;
}

static const uae_u8 *execscsicmd_out (int unitnum, const uae_u8 *cmd_data, int cmd_len)
{
    int error;
    struct IOContext *ioc = get_iocontext (unitnum);

    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_out\n", unitnum);

    error = execscsicmd (get_iocontext (unitnum), cmd_data, cmd_len, 0, 0, 0);

    if (error == 0)
	return cmd_data;

    return 0;
}

static const uae_u8 *execscsicmd_in (int unitnum, const uae_u8 *cmd_data, int cmd_len, int *outlen)
{
    int error;
    struct IOContext *ioc = get_iocontext (unitnum);

    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_in\n", unitnum);

    error = execscsicmd (ioc, cmd_data, cmd_len, ioc->scsibuf, DEVICE_SCSI_BUFSIZE, outlen);

    if (error == 0)
	return ioc->scsibuf;

    return 0;
}

static int execscsicmd_direct (int unitnum, uaecptr acmd)
{
    int sactual = 0;
    struct IOContext *ioc   =  get_iocontext (unitnum);
    struct IOStdReq  *ioreq =  ioc->ioreq;
    struct SCSICmd   *cmd   = &(ioc->cmd);

    uaecptr scsi_data         = get_long (acmd + 0);
    uae_u32 scsi_len          = get_long (acmd + 4);
    uaecptr scsi_cmd          = get_long (acmd + 12);
    int     scsi_cmd_len      = get_word (acmd + 16);
    int     scsi_cmd_len_orig = scsi_cmd_len;
    uae_u8  scsi_flags        = get_byte (acmd + 20);
    uaecptr scsi_sense        = get_long (acmd + 22);
    uae_u16 scsi_sense_len    = get_word (acmd + 26);
    int     io_error          = 0;
    int     parm;

    addrbank *bank_data    = &get_mem_bank (scsi_data);
    addrbank *bank_cmd	   = &get_mem_bank (scsi_cmd);

    uae_u8   *scsi_datap;
    uae_u8   *scsi_datap_org;

    DEBUG_LOG ("SCSIDEV: unit=%d: execscsicmd_direct\n", unitnum);

    /* do transfer directly to and from Amiga memory */
    if (!bank_data || !bank_data->check (scsi_data, scsi_len))
	return -5; /* IOERR_BADADDRESS */

    memset (cmd, 0, sizeof (cmd));

    ioreq->io_Length  = sizeof(struct SCSICmd);
    ioreq->io_Data    = (APTR)cmd;
    ioreq->io_Command = HD_SCSICMD;

    scsi_datap = scsi_datap_org
	= scsi_len ? bank_data->xlateaddr (scsi_data) : 0;

    cmd->scsi_Data        = (UWORD*) scsi_datap;
    cmd->scsi_Length      = scsi_len;
    cmd->scsi_Flags       = scsi_flags;
    cmd->scsi_Command     = bank_cmd->xlateaddr (scsi_cmd);
    cmd->scsi_CmdLength   = scsi_cmd_len;
    cmd->scsi_SenseData   = scsi_sense ? get_real_address (scsi_sense) : 0;
    cmd->scsi_SenseLength = scsi_sense_len;

    DEBUG_LOG ("SCSIDEV: sending command: 0x%2x\n", cmd->scsi_Command[0]);

    io_error = DoIO ((struct IORequest *)ioreq);

    DEBUG_LOG ("SCSIDEV: error: %d actual %d\n", io_error, cmd->scsi_Actual);

    gui_cd_led (1);

    put_long (acmd + 8,  cmd->scsi_Actual);
    put_word (acmd + 18, cmd->scsi_CmdActual);
    put_byte (acmd + 21, cmd->scsi_Status);
    put_byte (acmd + 28, cmd->scsi_SenseActual);

    return io_error;
}

static int check_device (const char *device, int unit)
{
    int result = 0;
    struct IOContext ioc;


    if (open_iocontext (device, unit, &ioc) == 0) {
	const uae_u8 INQUIRY_CMD[6] = {0x12,0,0,0,36,0};
	uae_u8 inqbuf[36];
	int outlen;

	memset (inqbuf, 0, sizeof inqbuf);

	if (execscsicmd (&ioc, INQUIRY_CMD, sizeof INQUIRY_CMD, inqbuf, sizeof inqbuf, &outlen) == 0) {
	    int         type     =                 inqbuf[0] & 0x1F;
	    const char *vendor   = (const char *) &inqbuf[8];
	    const char *prod_id  = (const char *) &inqbuf[16];
	    const char *prod_rev = (const char *) &inqbuf[32];

	    write_log ("%-16.16s %3d: '%-8.8s' '%-16.16s' '%-4.4s' %s\n",
                       device, unit, vendor, prod_id, prod_rev, type == 5 ? "CD-ROM" : "");
	    if (type == 5)
		result = 1;

	    close_iocontext (&ioc);
	}
    }

    return result;
}

static int media_check (int unitnum)
{
    int media;
    const uae_u8 TEST_UNIT_READY_CMD[6] = {0,0,0,0,0,0};

    media = execscsicmd_out (unitnum, TEST_UNIT_READY_CMD, sizeof(TEST_UNIT_READY_CMD)) ? 1 : 0;

    DEBUG_LOG ("SCSIDEV: media check :%d\n", media);

    return media;
}


static int open_scsi_device (int unitnum)
{
    int result = 0;

    DEBUG_LOG ("SCSIDEV: unit=%d: open_scsi_device\n", unitnum);

    if (unitnum < total_drives) {
        struct scsidevdata *sdd = &drives[unitnum];
	struct IOContext   *ioc = &(sdd->main_ioc);

        if (ioc->ioreq) {
	    DEBUG_LOG ("Already open\n");

	    result = 1;
	} else {
	    if (!open_iocontext (sdd->device, sdd->unit, ioc))
		result = 1;
	}
    }

    return result;
}

static void close_scsi_device (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: close_scsi_device\n", unitnum);

    if (unitnum < total_drives) {
        struct scsidevdata *sdd  = &drives[unitnum];
        struct Task        *self = FindTask (NULL);

        if (self == sdd->main_task)
            close_iocontext (&(sdd->main_ioc));
    }
}

static int open_device_thread (int unitnum)
{
    int result = 0;

    DEBUG_LOG ("SCSIDEV: unit=%d: open_device_thread\n", unitnum);

    if (unitnum < total_drives) {
	struct scsidevdata *sdd = &drives[unitnum];
	struct IOContext   *ioc = &(sdd->local_ioc);

	if (ioc->ioreq) {
	    result = 1;

	    if (sdd->local_task == FindTask (NULL)) {
		DEBUG_LOG ("Already open\n");
	    } else {
		write_log ("SCSIDEV: Warning: Orphaned I/O context.\n");
		open_iocontext (sdd->device, sdd->unit, ioc);
		sdd->local_task = FindTask (NULL);
	    }
	} else {
	    if (!open_iocontext (sdd->device, sdd->unit, ioc)) {
		result = 1;
		sdd->local_task = FindTask (NULL);
	    }
	}
    }

    return result;
}

static void close_device_thread (int unitnum)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: close_device_thread\n", unitnum);

    if (unitnum < total_drives) {
	struct scsidevdata *sdd  = &drives[unitnum];
	struct Task        *self = FindTask (NULL);

        if(sdd->local_ioc.ioreq) {
	    if (self == sdd->local_task) {
		close_iocontext (&(sdd->local_ioc));
		sdd->local_task = NULL;
	    } else
		write_log ("SCSIDEV: Warning: Failed to close local I/O context.\n");
	}
    }
}

static void close_scsi_bus (void)
{
    DEBUG_LOG ("SCSIDEV: close_scsi_bus\n");

    /* Not currently used */
}

#ifdef __amigaos4__
static int find_devices (void)
{
    struct FileSystemData *filesys;
    char cd_device[8];
    int i;

    /* block 'please insert volume CDx: requesters */
    struct Process *self = (struct Process *) FindTask (NULL);
    APTR old_windptr = self->pr_WindowPtr;
    self->pr_WindowPtr = (APTR)-1;

    /* Scan for device names of the form CDx: */
    for (i=0; i<10; i++) {
	sprintf (cd_device, "CD%d:\n", i);

	DEBUG_LOG ("Looking for %s\n", cd_device);

	if ((filesys = GetDiskFileSystemData (cd_device))) {
	    if (check_device (filesys->fsd_DeviceName, filesys->fsd_DeviceUnit))
		add_device (filesys->fsd_DeviceName, filesys->fsd_DeviceUnit);

	    FreeDiskFileSystemData (filesys);
	}
    }

    self->pr_WindowPtr = old_windptr;

    return total_drives;
}
#else
#include <exec/lists.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <string.h>

#ifndef NEWLIST
#define NEWLIST NewList
#endif

#ifndef ADDTAIL
#define ADDTAIL AddTail
#endif

struct MyDevNode
{
    struct MinNode node;
    UBYTE DevName[256];
    ULONG DevUnit;
};

static int find_devices (void)
{
    struct DosList *dl;
    struct MyDevNode *mdn, *mdn2;
    struct MinList mdl;

    NEWLIST ((struct List *)&mdl);
    dl = LockDosList (LDF_DEVICES | LDF_READ);

    while ((dl = NextDosEntry (dl, LDF_DEVICES))) {
	struct DeviceNode *dn = (struct DeviceNode *) dl;
	struct FileSysStartupMsg *fs = BADDR (dn->dn_Startup);

	if (TypeOfMem (fs)) {
	    UBYTE *devname = BADDR (fs->fssm_Device);

	    if (TypeOfMem (devname) && devname[0] != 0) {
		mdn2 = AllocVec (sizeof(*mdn2), MEMF_PUBLIC);

		for (mdn = (struct MyDevNode *)mdl.mlh_Head; mdn->node.mln_Succ;
		     mdn = (struct MyDevNode *)mdn->node.mln_Succ) {

		    if (strncmp (mdn->DevName, devname+1, devname[0]) == 0
			&& mdn->DevUnit == fs->fssm_Unit) {

			FreeVec (mdn2);
			mdn2 = NULL;
			break;
		    }
		}

		if (mdn2) {
		    memcpy (mdn2->DevName, devname+1, devname[0]);
		    mdn2->DevName[devname[0]] = 0;
		    mdn2->DevUnit = fs->fssm_Unit;
		    ADDTAIL ((struct List *)&mdl, (struct Node *)&mdn2->node);
		}
	    }
	}
    }

    UnLockDosList (LDF_DEVICES | LDF_READ);

    for (mdn  = (struct MyDevNode *)mdl.mlh_Head;
	 mdn2 = (struct MyDevNode *)mdn->node.mln_Succ; (mdn = mdn2)) {

	if (check_device (mdn->DevName, mdn->DevUnit))
	    add_device (mdn->DevName, mdn->DevUnit);

	FreeVec(mdn);
    }

    return total_drives;
}
#endif

static int open_scsi_bus (int flags)
{
    int num_devices = 0;

    DEBUG_LOG ("SCSIDEV: open_scsi_bus\n");

    num_devices = find_devices ();

    write_log ("SCSIDEV: %d device(s) found\n", num_devices);

    return num_devices;
}

static struct device_info *info_device (int unitnum, struct device_info *di)
{
    DEBUG_LOG ("SCSIDEV: unit=%d: info_device\n", unitnum);

    if (unitnum < total_drives) {
	struct scsidevdata *sdd = &drives[unitnum];

	di->bus		    = 0;
	di->target	    = unitnum;
	di->lun		    = 0;
	di->media_inserted  = media_check (unitnum);
	di->write_protected = 1;
	di->bytespersector  = 2048;
	di->cylinders	    = 1;
	di->type	    = INQ_ROMD; /* We only support CD/DVD drives for now */
	di->id		    = unitnum + 1;

	snprintf (di->label, 60, "%s:%d", sdd->device, sdd->unit);
    } else
	di = 0;

    return di;
}

static int check_isatapi (int unitnum)
{
    return 0; // FIXME
//    return drives[unitnum].isatapi;
}

struct device_functions devicefunc_scsi_amiga = {
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
    open_device_thread,
    close_device_thread
};
