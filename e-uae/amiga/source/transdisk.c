/*
 * Transdisk V4.2
 * Copyright 1995-97 Bernd Schmidt, Marcus Sundberg, Stefan Ropke,
 *                   Rodney Hester, Joanne Dow
 *
 * Use DICE and 2.0 includes or above to compile. SAS C should work too.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>

#include <exec/devices.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <devices/trackdisk.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>

static void usage(void);

static void usage(void)
{
    fprintf(stderr, "Usage: transdisk options\n");
    fprintf(stderr, "Recognized options:\n");
    fprintf(stderr, "-h: Assume device is high density\n");
    fprintf(stderr, "-d device unit: Use this device instead of DF0:\n");
    fprintf(stderr, "-s n: Begin transfer at track n\n");
    fprintf(stderr, "-e n: End transfer at track n\n");
    fprintf(stderr, "-w filename: writes the ADF file <filename> to disk\n\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "transdisk >RAM:df1.adf.1 -d trackdisk 1 -s 0 -e 39\n");
    fprintf(stderr, "transfers the first half of the floppy in DF1: into\n");
    fprintf(stderr, "a file in the RAM disk.\n");
    fprintf(stderr, "Or:\n");
    fprintf(stderr, "transdisk -w test.adf\n");
    fprintf(stderr, "writes the ADF-file test.adf to the disk in df0:\n");
}

int main(int argc, char **argv)
{
    char *filename, *openMode="rb";
    FILE *ADFFile;
    int write=0;
    struct IOStdReq *ioreq;
    struct MsgPort *port;

    UBYTE *buffer;
    char devicebuf[256];
    char *devicename = "trackdisk.device";
    char devicenum = 0;
    int i;
    int starttr = 0, endtr = 79;
    int sectors = 11;

    for (i = 1; i < argc;) {
	if (argv[i][0] != '-' || argv[i][2] != 0) {
	    usage();
	    exit(1);
	}
	switch (argv[i][1]) {
	 case 'h':
	    sectors = 22;
	    i++;
	    break;
	case 'd':
	    if (i+2 >= argc) {
		usage();
		exit(1);
	    }
	    devicenum = atoi(argv[i+2]);
	    sprintf(devicebuf, "%s.device", argv[i+1]);
	    devicename = devicebuf;
	    i += 3;
	    break;
	case 's':
	    if (i+1 >= argc) {
		usage();
		exit(1);
	    }
	    starttr = atoi(argv[i+1]);
	    i += 2;
	    break;
	case 'e':
	    if (i+1 >= argc) {
		usage();
		exit(1);
	    }
	    endtr = atoi(argv[i+1]);
	    i += 2;
	    break;
	case 'w':
	    if (i+1 >= argc) {
		usage();
		exit(1);
	    }
	    filename=argv[i+1];
	    write=1;
	    i += 2;
	    break;
	default:
	    usage();
	    exit(1);
	}
    }
    fprintf(stderr,"Using %s unit %d\n", devicename, devicenum);
    fprintf(stderr,"Tracks are %d sectors\n", sectors);
    fprintf(stderr,"First track %d, last track %d\n", starttr, endtr);

    buffer = AllocMem(512, MEMF_CHIP);
    if (write) {
	ADFFile = fopen(filename,openMode);

	if (!ADFFile) {
	    fprintf(stderr,"Error while opening input file\n");
	    exit (1);
	}
    }
    
    port = CreatePort(0, 0);
    if (port) {
	ioreq = CreateStdIO(port);
	if (ioreq) {
	    if (OpenDevice(devicename, devicenum, (struct IORequest *) ioreq, 0) == 0) {
		int tr, sec;

		ioreq->io_Command = write ? CMD_WRITE : CMD_READ;
		ioreq->io_Length = 512;
		ioreq->io_Data = buffer;
		for (tr = starttr*2; tr < (endtr+1)*2; tr++) {
		    fprintf(stderr,"Track: %d\r",tr/2);
		    for (sec = 0; sec < sectors; sec++) {
			fflush(stderr);
			if (write)
			    if (fread(buffer, sizeof(UBYTE), 512, ADFFile) < 512) {
				fprintf(stderr, "Error: ADF file to short?\n");
				exit(1);
			    }  
			ioreq->io_Offset = 512 * (tr * sectors + sec);
			DoIO( (struct IORequest *) ioreq);
			if (!write)
			    fwrite(buffer, sizeof(UBYTE), 512, stdout);
		    }
		}
		if (write) {		/* Make sure the last track is written to disk */
		    ioreq->io_Command = CMD_UPDATE;
		    DoIO( (struct IORequest *) ioreq);
		}
		ioreq->io_Command = TD_MOTOR;	/* Turn Disk-motor off */
		ioreq->io_Length = 0;
		DoIO( (struct IORequest *) ioreq);
		CloseDevice( (struct IORequest *) ioreq);
	    } else
		fprintf(stderr,"Unable to open %s unit %d\n", devicename, devicenum);
	    DeleteStdIO(ioreq);
	}
	DeletePort(port);
    }
    fprintf(stderr,"\n");
    FreeMem(buffer, 512);
    if (write)
	fclose (ADFFile);
    return 0;
}
