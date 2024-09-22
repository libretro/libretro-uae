#ifndef LIBRETRO_GLUE_H
#define LIBRETRO_GLUE_H

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "xwin.h"

extern struct vidbuf_description *gfxvidinfo;

#include <stdio.h>
#include "libretro-core.h"

#ifdef WITH_CHD
/*** CHD ***/
#include "deps/libz/zlib.h"
#include "deps/zstd/lib/zstd.h"
#include "deps/7zip/LzmaDec.h"

#include "libchdr/chd.h"
#include "libchdr/cdrom.h"
#include "libchdr/flac.h"
#include "archivers/chd/chdtypes.h"

#define METADATA_HEADER_SIZE        16
#define MAX_ZLIB_ALLOCS             64
#define MAX_LZMA_ALLOCS             64

typedef UINT32 chd_codec_type;
typedef UINT32 chd_metadata_tag;

typedef struct cdrom_track_info
{
	/* fields used by CHDMAN and in MAME */
	UINT32 trktype;     /* track type */
	UINT32 subtype;     /* subcode data type */
	UINT32 datasize;    /* size of data in each sector of this track */
	UINT32 subsize;     /* size of subchannel data in each sector of this track */
	UINT32 frames;      /* number of frames in this track */
	UINT32 extraframes; /* number of "spillage" frames in this track */
	UINT32 pregap;      /* number of pregap frames */
	UINT32 postgap;     /* number of postgap frames */
	UINT32 pgtype;      /* type of sectors in pregap */
	UINT32 pgsub;       /* type of subchannel data in pregap */
	UINT32 pgdatasize;  /* size of data in each sector of the pregap */
	UINT32 pgsubsize;   /* size of subchannel data in each sector of the pregap */

	/* fields used in CHDMAN only */
	UINT32 padframes;   /* number of frames of padding to add to the end of the track; needed for GDI */

	/* fields used in MAME/MESS only */
	UINT32 logframeofs; /* logical frame of actual track data - offset by pregap size if pregap not physically present */
	UINT32 physframeofs;/* physical frame of actual track data in CHD data */
	UINT32 chdframeofs; /* frame number this track starts at on the CHD */
	UINT32 logframes;   /* number of frames from logframeofs until end of track data */
} cdrom_track_info;

typedef struct cdrom_toc
{
	UINT32 numtrks;     /* number of tracks */
	UINT32 flags;       /* see FLAG_ above */
	cdrom_track_info tracks[CD_MAX_TRACKS];
} cdrom_toc;

typedef struct cdrom_file
{
	chd_file *          chd;                /* CHD file */
	cdrom_toc           cdtoc;              /* TOC for the CD */
#if 0
	chdcd_track_input_info track_info;      /* track info */
#endif
	core_file *         fhandle[CD_MAX_TRACKS];/* file handle */
} cdrom_file;


/* interface to a codec */
typedef struct _codec_interface codec_interface;
struct _codec_interface
{
	UINT32		compression;								/* type of compression */
	const char *compname;									/* name of the algorithm */
	UINT8		lossy;										/* is this a lossy algorithm? */
	chd_error	(*init)(void *codec, UINT32 hunkbytes);		/* codec initialize */
	void		(*free)(void *codec);						/* codec free */
	chd_error	(*decompress)(void *codec, const uint8_t *src, uint32_t complen, uint8_t *dest, uint32_t destlen); /* decompress data */
	chd_error	(*config)(void *codec, int param, void *config); /* configure */
};

/* a single map entry */
typedef struct _map_entry map_entry;
struct _map_entry
{
	uint64_t            offset;         /* offset within the file of the data */
	uint32_t            crc;            /* 32-bit CRC of the data */
	uint32_t            length;         /* length of the data */
	uint8_t             flags;          /* misc flags */
};

/* a single metadata entry */
typedef struct _metadata_entry metadata_entry;
struct _metadata_entry
{
	uint64_t            offset;         /* offset within the file of the header */
	uint64_t            next;           /* offset within the file of the next header */
	uint64_t            prev;           /* offset within the file of the previous header */
	uint32_t            length;         /* length of the metadata */
	uint32_t            metatag;        /* metadata tag */
	uint8_t             flags;          /* flag bits */
};

/* codec-private data for the ZLIB codec */
typedef struct _zlib_allocator zlib_allocator;
struct _zlib_allocator
{
	uint32_t *				allocptr[MAX_ZLIB_ALLOCS];
	uint32_t *				allocptr2[MAX_ZLIB_ALLOCS];
};

typedef struct _zlib_codec_data zlib_codec_data;
struct _zlib_codec_data
{
	z_stream				inflater;
	zlib_allocator			allocator;
};

/* codec-private data for the LZMA codec */
typedef struct _lzma_allocator lzma_allocator;
struct _lzma_allocator
{
	void *(*Alloc)(void *p, size_t size);
 	void (*Free)(void *p, void *address); /* address can be 0 */
	void (*FreeSz)(void *p, void *address, size_t size); /* address can be 0 */
	uint32_t*	allocptr[MAX_LZMA_ALLOCS];
	uint32_t*	allocptr2[MAX_LZMA_ALLOCS];
};

typedef struct _lzma_codec_data lzma_codec_data;
struct _lzma_codec_data 
{
	CLzmaDec		decoder;
	lzma_allocator	allocator;
};

typedef struct _huff_codec_data huff_codec_data;
struct _huff_codec_data
{
	struct huffman_decoder* decoder;
};

typedef struct _zstd_codec_data zstd_codec_data;
struct _zstd_codec_data
{
	ZSTD_DStream *dstream;
};

/* codec-private data for the CDZL codec */
typedef struct _cdzl_codec_data cdzl_codec_data;
struct _cdzl_codec_data {
	/* internal state */
	zlib_codec_data		base_decompressor;
#ifdef WANT_SUBCODE
	zlib_codec_data		subcode_decompressor;
#endif
	uint8_t*			buffer;
};

/* codec-private data for the CDLZ codec */
typedef struct _cdlz_codec_data cdlz_codec_data;
struct _cdlz_codec_data {
	/* internal state */
	lzma_codec_data		base_decompressor;
#ifdef WANT_SUBCODE
	zlib_codec_data		subcode_decompressor;
#endif
	uint8_t*			buffer;
};

/* codec-private data for the FLAC codec */
typedef struct _flac_codec_data flac_codec_data;
struct _flac_codec_data {
	/* internal state */
	int		native_endian;
	flac_decoder	decoder;
};

/* codec-private data for the CDFL codec */
typedef struct _cdfl_codec_data cdfl_codec_data;
struct _cdfl_codec_data {
	/* internal state */
	int		swap_endian;
	flac_decoder	decoder;
#ifdef WANT_SUBCODE
	zlib_codec_data		subcode_decompressor;
#endif
	uint8_t*	buffer;
};

typedef struct _cdzs_codec_data cdzs_codec_data;
struct _cdzs_codec_data
{
	zstd_codec_data base_decompressor;
#ifdef WANT_SUBCODE
	zstd_codec_data subcode_decompressor;
#endif
	uint8_t*				buffer;
};

/* internal representation of an open CHD file */
struct _chd_file
{
	uint32_t                cookie;			/* cookie, should equal COOKIE_VALUE */

	core_file *             file;			/* handle to the open core file */
	chd_header              header;			/* header, extracted from file */

	chd_file *              parent;			/* pointer to parent file, or NULL */

	map_entry *             map;			/* array of map entries */

#ifdef NEED_CACHE_HUNK
	uint8_t *               cache;			/* hunk cache pointer */
	uint32_t                cachehunk;		/* index of currently cached hunk */

	uint8_t *               compare;		/* hunk compare pointer */
	uint32_t                comparehunk;	/* index of current compare data */
#endif

	uint8_t *               compressed;		/* pointer to buffer for compressed data */
	const codec_interface * codecintf[4];	/* interface to the codec */

	zlib_codec_data         zlib_codec_data;		/* zlib codec data */
	lzma_codec_data         lzma_codec_data;		/* lzma codec data */
	huff_codec_data         huff_codec_data;		/* huff codec data */
	flac_codec_data         flac_codec_data;		/* flac codec data */
	zstd_codec_data         zstd_codec_data;		/* zstd codec data */
	cdzl_codec_data         cdzl_codec_data;		/* cdzl codec data */
	cdlz_codec_data         cdlz_codec_data;		/* cdlz codec data */
	cdfl_codec_data         cdfl_codec_data;		/* cdfl codec data */
	cdzs_codec_data         cdzs_codec_data;		/* cdzs codec data */

#ifdef NEED_CACHE_HUNK
	uint32_t                maxhunk;		/* maximum hunk accessed */
#endif

	uint8_t *               file_cache;		/* cache of underlying file */
};

chd_error chd_hunk_info(chd_file *chd, UINT32 hunknum, chd_codec_type *compressor, UINT32 *compbytes);
chd_error read_partial_sector(cdrom_file *file, void *dest, UINT32 lbasector, UINT32 chdsector, UINT32 tracknum, UINT32 startoffs, UINT32 length, bool phys);
chd_error cdrom_parse_metadata(chd_file *chd, cdrom_toc *toc);
chd_error chd_read_metadata(chd_file *chd, chd_metadata_tag searchtag, UINT32 searchindex, char *output);
chd_error metadata_find_entry(chd_file *chd, UINT32 metatag, UINT32 metaindex, metadata_entry *metaentry);

/* base functionality */
cdrom_file *cdrom_open(chd_file *chd);
void cdrom_close(cdrom_file *file);

/* core read access */
UINT32 cdrom_read_data(cdrom_file *file, UINT32 lbasector, void *buffer, UINT32 datatype, bool phys);
UINT32 cdrom_read_subcode(cdrom_file *file, UINT32 lbasector, void *buffer, bool phys);

/* handy utilities */
UINT32 cdrom_get_track(cdrom_file *file, UINT32 frame);
UINT32 cdrom_get_track_start(cdrom_file *file, UINT32 track);
UINT32 cdrom_get_track_start_phys(cdrom_file *file, UINT32 track);

/* TOC utilities */
int cdrom_get_last_track(cdrom_file *file);
int cdrom_get_adr_control(cdrom_file *file, int track);
int cdrom_get_track_type(cdrom_file *file, int track);
const cdrom_toc *cdrom_get_toc(cdrom_file *file);
/*** CHD ***/
#endif

/* zlib */
#include "deps/libz/zlib.h"
#include "deps/libz/unzip.h"
void gz_compress(const char *in, const char *out);
void gz_uncompress(const char *in, const char *out);
void zip_uncompress(const char *in, const char *out, char *lastfile);

/* 7z */
#include "deps/7zip/7z.h"
#include "deps/7zip/7zBuf.h"
#include "deps/7zip/7zCrc.h"
#include "deps/7zip/7zFile.h"
#include "deps/7zip/7zTypes.h"
void sevenzip_uncompress(const char *in, const char *out, char *lastfile);

/* HDF */
int make_hdf (char *hdf_path, char *hdf_size, char *device_name);

/* Misc */
int qstrcmp(const void *a, const void *b);
int retro_remove(const char *path);
int remove_recurse(const char *path);
int fcopy(const char *src, const char *dst);
int fcmp(const char *src, const char *dst);
uint64_t fsize(const char *path);

/* String helpers functions */
char* trimwhitespace(char *str);
char* strleft(const char* str, int len);
char* strright(const char* str, int len);
bool strstartswith(const char* str, const char* start);
bool strendswith(const char* str, const char* end);
void path_join(char* out, const char* basedir, const char* filename);

#endif /* LIBRETRO_GLUE_H */
