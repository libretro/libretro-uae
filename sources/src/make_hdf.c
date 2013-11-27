 /*
  * E-UAE - The portable Amiga Emulator
  *
  * make_hdf
  *
  * A quick hack to generate a hard file image and associated config option.
  * Anybody want to show this rubbish some love, feel free.
  *
  * Copyright Richard Drummond 2007
  *
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <ctype.h>

static void print_help (void)
{
   fprintf (stderr, "make_hdf <path> <size> [<device>]\n");
   fprintf (stderr, "\n");
   fprintf (stderr, "<path>   = file path to hdf image to create\n");
   fprintf (stderr, "<size>   = size of image to create in MB\n");
   fprintf (stderr, "           follow <size> by G to specify size in GB\n");
   fprintf (stderr, "<device> = device name to include in config option\n");
   fprintf (stderr, "           defaults to DH0 if omitted\n");
}

static int create_hdf (const char *path, off_t size)
{
    FILE *f;
    void *buf;
    const size_t CHUNK_SIZE = 4096;

    f = fopen (path, "wb+");

    if (f) {
	if (size == 0) {
	    fclose (f);
	    return 0;
	}

	/*
	 * Try it the easy way.
	 */
	if (fseeko (f, size - 1, SEEK_SET) == 0) {
	    fputc (0, f);
	    if (fseeko (f, 0, SEEK_SET) == 0) {
		fclose (f);
		return 0;
	    }
	}

	/*
	 * Okay. That failed. Let's assume seeking passed
	 * the end of a file ain't supported. Do it the
	 * hard way.
	 */
	fseeko (f, 0, SEEK_SET);
	buf = calloc (1, CHUNK_SIZE);

	while (size >= (off_t) CHUNK_SIZE) {
	    if (fwrite (buf, CHUNK_SIZE, 1, f) != 1)
		break;
	    size -= CHUNK_SIZE;
	}

	if (size < (off_t) CHUNK_SIZE) {
	    if (size == 0 || fwrite (buf, (size_t)size, 1, f) == 1) {
		fclose (f);
		return 0;
	    }
	}
    }

    perror ("Failed creating hdf");

    if (f) {
	fclose (f);
	/* TODO: Should probably delete failed image here. */
    }

    return -1;
}

int main (int argc, char *argv[])
{
    const char *hdf_path;
    const char *device_name = "DH0";

    uae_u64 size;
    char *size_spec;

    uae_u32 block_size = 512;
    uae_u64 num_blocks;

    uae_u32 cylinders;
    uae_u32 blocks_per_track;
    uae_u32 surfaces;

    if (argc < 3 || strcmp (argv[1], "-h") == 0) {
	print_help ();
	exit (EXIT_SUCCESS);
    }

    hdf_path = argv[1];
    size = strtoll(argv[2], &size_spec, 10);
    if (argv[3])
	device_name = argv[3];

    /* Munge size specifier */
    if (size > 0) {
	char c = (toupper(*size_spec));

	if (c == 'K')
	    size *= 1024;
	else if (c == 'M' || c == '\0')
	    size *= 1024 * 1024;
	else if (c == 'G')
	    size *= 1024 * 1024 * 1024;
	else
	    size = 0;
    }

    if (size <= 0) {
	fprintf (stderr, "Invalid size\n");
	exit (EXIT_FAILURE);
    }

    if ((size >= (1LL << 31)) && (sizeof (off_t) < sizeof (uae_u64))) {
	fprintf (stderr, "Specified size too large (2GB file size is maximum).\n");
	exit (EXIT_FAILURE);
    }

    num_blocks = size / block_size;

    /* We don't want more than (2^32)-1 blocks */
    if (num_blocks >= (1LL << 32)) {
	fprintf (stderr, "Specified size too large (too many blocks).\n");
	exit (EXIT_FAILURE);
    }

    /*
     * Try and work out some plausible geometry
     *
     * We try and set surfaces and blocks_per_track to keep
     * cylinders < 65536. Prior to OS 3.9, FFS had problems with
     * more cylinders than that.
     */

    /* The default practice in UAE hardfiles, so let's start there. */
    blocks_per_track = 32;
    surfaces = 1;

    cylinders = num_blocks / (blocks_per_track * surfaces);

    if (cylinders == 0) {
	fprintf (stderr, "Specified size is too small.\n");
	exit (EXIT_FAILURE);
    }

    while (cylinders > 65535 && surfaces < 255) {
	surfaces++;
	cylinders = num_blocks / (blocks_per_track * surfaces);
    }

    while (cylinders > 65535 && blocks_per_track < 255) {
	blocks_per_track++;
	cylinders = num_blocks / (blocks_per_track * surfaces);
    }

    /* Calculate size based on above geometry */
    num_blocks = (uae_u64)cylinders * surfaces * blocks_per_track;

    /* make file */
    if (create_hdf (hdf_path, num_blocks * block_size) < 0)
	return EXIT_FAILURE;

    /* output_spec */
    printf ("hardfile2=rw,%s:%s,%d,%d,%d,%d,%d,\n", device_name, hdf_path, blocks_per_track, surfaces, 2, block_size, 0);
    printf ("hardfile=rw,%d,%d,%d,%d,%s\n", blocks_per_track, surfaces, 2, block_size, hdf_path);

    return EXIT_SUCCESS;
}
