 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Drive Click Emulation Support Functions
  *
  * Copyright 2004 James Bagg, Toni Wilen
  */

#include "sysconfig.h"
#include "sysdeps.h"

#ifdef DRIVESOUND

#include "uae.h"
#include "options.h"
#include "driveclick.h"
#include "sounddep/sound.h"
#include "zfile.h"
#include "events.h"


#include "fsdb.h"

#include "resource/drive_click.c"
#include "resource/drive_snatch.c"
#include "resource/drive_spin.c"
#include "resource/drive_startup.c"



static struct drvsample drvs[4][DS_END];
static int freq = 44100;

static int drv_starting[4], drv_spinning[4], drv_has_spun[4], drv_has_disk[4];

static int click_initialized;
#define DS_SHIFT 10
static int sample_step;

static uae_s16 *clickbuffer;

uae_s16 *decodewav (uae_u8 *s, int *lenp)
{
    uae_s16 *dst;
    uae_u8 *src = s;
    int len;

    if (memcmp (s, "RIFF", 4))
	return 0;
    if (memcmp (s + 8, "WAVE", 4))
	return 0;
    s += 12;
    len = *lenp;
    while (s < src + len) {
	if (!memcmp (s, "fmt ", 4))
	    freq = s[8 + 4] | (s[8 + 5] << 8);
	if (!memcmp (s, "data", 4)) {
	    s += 4;
	    len = s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24);
	    dst = xmalloc (len);
#ifdef WORDS_BIGENDIAN 
	    {
 		int i;
		uae_u8* d = (uae_u8*) dst;
		for (i = 0; i < len; i+= 2) {
			d[i] = s[i + 5];
			d[i+1] = s[i + 4];
		}
	    }
#else
	    memcpy (dst, s + 4, len);
#endif	    
	    *lenp = len / 2;
	    return dst;
	}
	s += 8 + (s[4] | (s[5] << 8) | (s[6] << 16) | (s[7] << 24));
    }
    return 0;
}

static int loadsample (const char *path, struct drvsample *ds)
{
    struct zfile *f;
    uae_u8 *buf;
    int size;
    int i;

    f = zfile_fopen (path, "rb");
    if (!f) {
	write_log ("driveclick: can't open '%s'\n", path);
	return 0;
    }
    zfile_fseek (f, 0, SEEK_END);
    size = zfile_ftell (f);
    buf = malloc (size);
    zfile_fseek (f, 0, SEEK_SET);
    zfile_fread (buf, size, 1, f);
    zfile_fclose (f);
    
    /*
    printf("size=%i \n", size);
    for (i = 0; i < size; i++) {
    	printf("0x%02x,", buf[i]);
    	if (i % 20 == 19) {
    		printf("\n");
    	}
    }
    printf("\n");
    flush(NULL);
    }
    */
    
    ds->len = size;
    ds->p = decodewav (buf, &ds->len);
    
    free (buf);
    return 1;
}

//The same function as load sample, but the wav data is already in memory.
//No read, just decode.
static int loadsample_resource(int resId, struct drvsample *ds) {
	uae_u8* buf  = NULL;
	int size;
	switch (resId) {
	case DS_CLICK: {
		buf = res_drive_click;
		size = res_drive_click_size;
	} break;
	case DS_SPIN :
	case DS_SPINND: {
		buf = res_drive_spin;
		size = res_drive_spin_size;
	} break;
	case DS_START: {
		buf = res_drive_startup;
		size = res_drive_startup_size;
	} break;
	case DS_SNATCH: {
		buf = res_drive_snatch;
		size = res_drive_snatch_size;
	} break;
	
	default: return 0;
	} //end of switch


	//write_log("loading click resource %i \n", resId); 
	ds->len = size;
	ds->p = decodewav (buf, &ds->len);
    
    return 1;
	
}

static void freesample (struct drvsample *s)
{
    free (s->p);
    s->p = 0;
}

extern char *start_path;

void driveclick_init (void)
{
    int v, vv, i, j;
    static char tmp_path[1024];

    driveclick_free ();
    vv = 0;

    write_log("driveclick init...\n");    

    for (i = 0; i < 4; i++) {
	if (currprefs.dfxclick[i]) {
	    /* TODO: Implement location of sample data */
		//load from resource
	    if (currprefs.dfxclick[i] > 0) { // > 0
			v  = loadsample_resource(DS_CLICK, &drvs[i][DS_CLICK]);
			v += loadsample_resource(DS_SPIN, &drvs[i][DS_SPIN]);
			v += loadsample_resource(DS_SPINND, &drvs[i][DS_SPINND]);
			v += loadsample_resource(DS_SNATCH, &drvs[i][DS_SNATCH]);
			v += loadsample_resource(DS_START, &drvs[i][DS_START]);
			
	    } else 
	    //load from file
	    if (currprefs.dfxclick[i] == -1) {
#ifdef __PS3__
			//currprefs.dfxclickexternal[i] is the path to the wav directory (path contains trailing separator)
			sprintf (tmp_path, "%sdrive_click.wav", currprefs.dfxclickexternal[i]);
			v = loadsample (tmp_path, &drvs[i][DS_CLICK]);
			sprintf (tmp_path, "%sdrive_spin.wav", currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SPIN]);
			sprintf (tmp_path, "%sdrive_spinnd.wav", currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SPINND]);
			sprintf (tmp_path, "%sdrive_startup.wav", currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_START]);
			sprintf (tmp_path, "%sdrive_snatch.wav", currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SNATCH]);
#else
			char * start_path = "."; //TODO - ??? set correct path
			sprintf (tmp_path, "%suae_data%cdrive_click_%s", start_path, FSDB_DIR_SEPARATOR, currprefs.dfxclickexternal[i]);
			v = loadsample (tmp_path, &drvs[i][DS_CLICK]);
			sprintf (tmp_path, "%suae_data%cdrive_spin_%s", start_path, FSDB_DIR_SEPARATOR, currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SPIN]);
			sprintf (tmp_path, "%suae_data%cdrive_spinnd_%s", start_path, FSDB_DIR_SEPARATOR, currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SPINND]);
			sprintf (tmp_path, "%suae_data%cdrive_startup_%s", start_path, FSDB_DIR_SEPARATOR, currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_START]);
			sprintf (tmp_path, "%suae_data%cdrive_snatch_%s", start_path, FSDB_DIR_SEPARATOR, currprefs.dfxclickexternal[i]);
			v += loadsample (tmp_path, &drvs[i][DS_SNATCH]);
#endif	/* __PS3__ */
	    }
	    if (v == 0) {
			int j;
			for (j = 0; j < DS_END; j++) {
		    	freesample (&drvs[i][j]);
		    }
			currprefs.dfxclick[i] = changed_prefs.dfxclick[i] = 0;
	    }
	    for (j = 0; j < DS_END; j++) {
			drvs[i][j].len <<= DS_SHIFT;
		}
	    drvs[i][DS_CLICK].pos = drvs[i][DS_CLICK].len;
	    drvs[i][DS_SNATCH].pos = drvs[i][DS_SNATCH].len;
	    vv += currprefs.dfxclick[i];

	}
    }
    if (vv > 0) {
    	write_log("reset driveclick \n"); 
		driveclick_reset ();
		click_initialized = 1;
    }
}

void driveclick_reset (void)
{
    free (clickbuffer);
    clickbuffer = xmalloc (sndbufsize);
    sample_step = (freq << DS_SHIFT) / currprefs.sound_freq;
}

void driveclick_free (void)
{
    int i, j;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < DS_END; j++)
	    freesample (&drvs[i][j]);
    }
    memset (drvs, 0, sizeof (drvs));
    free (clickbuffer);
    clickbuffer = 0;
    click_initialized = 0;
}

STATIC_INLINE uae_s16 getsample (void)
{
    uae_s32 smp = 0;
    int div = 0, i;

    for (i = 0; i < 4; i++) {
	if (currprefs.dfxclick[i]) {
	    struct drvsample *ds_start = &drvs[i][DS_START];
	    struct drvsample *ds_spin = drv_has_disk[i] ? &drvs[i][DS_SPIN] : &drvs[i][DS_SPINND];
	    struct drvsample *ds_click = &drvs[i][DS_CLICK];
	    struct drvsample *ds_snatch = &drvs[i][DS_SNATCH];
	    div += 2;
	    if (drv_spinning[i] || drv_starting[i]) {
		if (drv_starting[i] && drv_has_spun[i]) {
		    if (ds_start->p && ds_start->pos < ds_start->len) {
			smp = ds_start->p[ds_start->pos >> DS_SHIFT];
			ds_start->pos += sample_step;
		    } else {
			drv_starting[i] = 0;
		    }
		} else if (drv_starting[i] && drv_has_spun[i] == 0) {
		    if (ds_snatch->p && ds_snatch->pos < ds_snatch->len) {
			smp = ds_snatch->p[ds_snatch->pos >> DS_SHIFT];
			ds_snatch->pos += sample_step;
		    } else {
			drv_starting[i] = 0;
			ds_start->pos = ds_start->len;
			drv_has_spun[i] = 1;
		    }
		}
		if (ds_spin->p && drv_starting[i] == 0) {
		    if (ds_spin->pos >= ds_spin->len)
			ds_spin->pos -= ds_spin->len;
		    smp = ds_spin->p[ds_spin->pos >> DS_SHIFT];
		    ds_spin->pos += sample_step;
		}
	    }
	    if (ds_click->p && ds_click->pos < ds_click->len) {
		smp += ds_click->p[ds_click->pos >> DS_SHIFT];
		ds_click->pos += sample_step;
	    }
	}
    }
    if (!div)
	return 0;
    return smp / div;
}

static int clickcnt;

static void mix (void)
{
    int total = ((uae_u8*)sndbufpt - (uae_u8*)sndbuffer) / (currprefs.sound_stereo ? 4 : 2);

    if (currprefs.dfxclickvolume > 0) {
	while (clickcnt < total) {
	    clickbuffer[clickcnt++] = getsample() * (100 - currprefs.dfxclickvolume) / 100;
	}
    } else {
	while (clickcnt < total) {
	    clickbuffer[clickcnt++] = getsample();
	}
    }
}

STATIC_INLINE uae_s16 limit (uae_s32 v)
{
    if (v < -32768)
	v = -32768;
    if (v > 32767)
	v = 32767;
    return v;
}

void driveclick_mix (uae_s16 *sndbuffer, int size)
{
    int i;

    if (!click_initialized)
	return;
    mix();
    clickcnt = 0;
    if (currprefs.sound_stereo) {
        for (i = 0; i < size / 2; i++) {
	    uae_s16 s = clickbuffer[i];
	    sndbuffer[0] = limit(((sndbuffer[0] + s) * 2) / 3);
	    sndbuffer[1] = limit(((sndbuffer[1] + s) * 2) / 3);
	    sndbuffer += 2;
        }
    } else {
        for (i = 0; i < size; i++) {
	    sndbuffer[0] = limit(((sndbuffer[0] + clickbuffer[i]) * 2) / 3);
	    sndbuffer++;
	}
    }
}

void driveclick_click (int drive, int startOffset)
{
    if (!click_initialized)
	return;
    if (!currprefs.dfxclick[drive])
	return;
    mix ();
    drvs[drive][DS_CLICK].pos = (startOffset * 4) << DS_SHIFT;
    if (drvs[drive][DS_CLICK].pos > drvs[drive][DS_CLICK].len / 2)
	drvs[drive][DS_CLICK].pos = drvs[drive][DS_CLICK].len / 2;
}

void driveclick_motor (int drive, int running)
{
    if (!click_initialized)
	return;
    if (!currprefs.dfxclick[drive])
	return;
    mix();
    if (running == 0) {
	drv_starting[drive] = 0;
	drv_spinning[drive] = 0;
    } else {
        if (drv_spinning[drive] == 0) {
	    drv_starting[drive] = 1;
	    drv_spinning[drive] = 1;
	    if (drv_has_disk[drive] && drv_has_spun[drive] == 0 && drvs[drive][DS_SNATCH].pos >= drvs[drive][DS_SNATCH].len)
		drvs[drive][DS_SNATCH].pos = 0;
	    if (running == 2)
		drvs[drive][DS_START].pos = 0;
	    drvs[drive][DS_SPIN].pos = 0;
	}
    }
}

void driveclick_insert (int drive, int eject)
{
    if (!click_initialized)
	return;
    if (!currprefs.dfxclick[drive])
	return;
    if (eject)
	drv_has_spun[drive] = 0;
    drv_has_disk[drive] = !eject;
}

void driveclick_check_prefs (void)
{
    int i;

    if (currprefs.dfxclickvolume != changed_prefs.dfxclickvolume ||
	currprefs.dfxclick[0] != changed_prefs.dfxclick[0] ||
	currprefs.dfxclick[1] != changed_prefs.dfxclick[1] ||
	currprefs.dfxclick[2] != changed_prefs.dfxclick[2] ||
	currprefs.dfxclick[3] != changed_prefs.dfxclick[3] ||
	strcmp (currprefs.dfxclickexternal[0], changed_prefs.dfxclickexternal[0]) ||
	strcmp (currprefs.dfxclickexternal[1], changed_prefs.dfxclickexternal[1]) ||
	strcmp (currprefs.dfxclickexternal[2], changed_prefs.dfxclickexternal[2]) ||
	strcmp (currprefs.dfxclickexternal[3], changed_prefs.dfxclickexternal[3]))
    {
	currprefs.dfxclickvolume = changed_prefs.dfxclickvolume;
	for (i = 0; i < 4; i++) {
	    currprefs.dfxclick[i] = changed_prefs.dfxclick[i];
	    strcpy (currprefs.dfxclickexternal[i], changed_prefs.dfxclickexternal[i]);
	}
	driveclick_init ();
    }
}

#endif
