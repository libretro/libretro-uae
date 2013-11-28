/*
 * PUAE - The *nix Amiga Emulator
 *
 * QT GUI for PUAE
 *
 * Copyright 2010 Mustafa 'GnoStiC' TUFAN
 *
 */

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QThread>
#include "puae_bridge.h"
#include "puae_mainwindow.h"
//#include "ui_puae_mainwindow.h"

extern "C" {
#include "puae_registry.h"
#include "gui.h"
#include "options.h"
#include "writelog.h"
#include "misc.h"
extern struct uae_prefs workprefs;
extern struct uae_prefs currprefs;
extern void romlist_clear (void);
extern int load_keyring (struct uae_prefs *p, const char *path);
}

static unsigned int gui_available;
static int allow_quit;
static int restart_requested;
static int full_property_sheet = 1;
static struct uae_prefs *pguiprefs;
static int currentpage;
static int qs_request_reset;
static int qs_override;
int gui_active;

static int forceroms;

/*
 *
 *
 */
/*class MyThread : public QThread {
public:
	virtual void run();
};

void MyThread::run()
{
    QApplication a(NULL, NULL);
    puae_MainWindow w;
    w.show();
    a.exec();
}
*/

/* This function is called from od-macosx/main.m
 * WARNING: This gets called *before* real_main(...)!
 */
extern "C" void cocoa_gui_early_setup (void)
{
}

void read_rom_list (void)
{
	char tmp2[1000];
	int idx, idx2;
	char tmp[1000];
	int size, size2, exists;

	romlist_clear ();
	exists = regexiststree ("DetectedROMs");
	if (!exists || forceroms) {
		load_keyring (NULL, NULL);
		scan_roms (forceroms ? 0 : 1);
	}
	forceroms = 0;
}

static void prefs_to_gui (struct uae_prefs *p)
{
	workprefs = *p;
	/* filesys hack */
	workprefs.mountitems = currprefs.mountitems;
	memcpy (&workprefs.mountconfig, &currprefs.mountconfig, MOUNT_CONFIG_SIZE * sizeof (struct uaedev_config_info));

	//updatewinfsmode (&workprefs);
}

static void gui_to_prefs (void)
{
	/* Always copy our prefs to changed_prefs, ... */
	changed_prefs = workprefs;
	/* filesys hack */
	currprefs.mountitems = changed_prefs.mountitems;
	memcpy (&currprefs.mountconfig, &changed_prefs.mountconfig, MOUNT_CONFIG_SIZE * sizeof (struct uaedev_config_info));
	fixup_prefs (&changed_prefs);

	//updatewinfsmode (&changed_prefs);
}

int GetSettings (int all_options)
{
	static int init_called = 0;
	int first = 0;

	gui_active++;

	full_property_sheet = all_options;
	allow_quit = all_options;
	pguiprefs = &currprefs;
	memset (&workprefs, 0, sizeof (struct uae_prefs));
	default_prefs (&workprefs, 0);

	prefs_to_gui (&changed_prefs);

	if (!init_called) {
		first = 1;
		init_called = 1;
	}
	
	if (first)
		write_log ("Entering GUI idle loop\n");
	
	full_property_sheet = 0;
	gui_active--;
	return 0;
}

int gui_init (void)
{
//  MyThread *QT_GUI_Thread=new MyThread;
//	QT_GUI_Thread->start();
	int ret;
	
	read_rom_list ();
	inputdevice_updateconfig (&workprefs, &currprefs);
 
        for (;;) {
                ret = GetSettings (1);
                if (!restart_requested)
                        break;
                restart_requested = 0;
        }
#ifdef AVIOUTPUT
        if (ret > 0) {
                AVIOutput_Begin ();
        }
#endif
        return ret;
}

void gui_exit (void)
{

}

int gui_update (void)
{
	return 1;
}

void gui_display (int shortcut)
{
	int foo;

	if (shortcut == -1) {
		int local_argc = 0;
		char **local_argv = NULL;
		QApplication myApp(local_argc, local_argv);
		puae_MainWindow w;

		w.show();
		foo = myApp.exec();
	}
}

extern "C" void gui_message (const char *format,...)
{
	char msg[2048];
	va_list parms;

	va_start (parms, format);
	vsprintf (msg, format, parms);
	va_end (parms);

	if (gui_available)
		QMessageBox::information(0, "PUAE", msg);

//	write_log (msg);
}

void gui_fps (int fps, int idle, int color)
{
        gui_data.fps = fps;
        gui_data.idle = idle;
        gui_led (LED_FPS, 0);
        gui_led (LED_CPU, 0);
        gui_led (LED_SND, (gui_data.sndbuf_status > 1 || gui_data.sndbuf_status < 0) ? 0 : 1);
}

void gui_handle_events (void)
{
}

void gui_led (int led, int on)
{
	int writing = 0, playing = 0, active2 = 0;
	int center = 0;

	if (led >= LED_DF0 && led <= LED_DF3) {
	} else if (led == LED_POWER) {
	} else if (led == LED_HD) {
		if (on > 1)
			writing = 1;
	} else if (led == LED_CD) {
		if (on & LED_CD_AUDIO)
			playing = 1;
		else if (on & LED_CD_ACTIVE2)
			active2 = 1;
		on &= 1;
	} else if (led == LED_FPS) {
		double fps = (double)gui_data.fps / 10.0;
		if (fps > 999.9)
			fps = 999.9;
	} else if (led == LED_CPU) {
	} else if (led == LED_SND && gui_data.drive_disabled[3]) {
	} else if (led == LED_MD) {
	}

}

static void gui_flicker_led2 (int led, int unitnum, int status)
{
        static int resetcounter[LED_MAX];
        uae_s8 old;
        uae_s8 *p;

        if (led == LED_HD)
                p = &gui_data.hd;
        else if (led == LED_CD)
                p = &gui_data.cd;
        else if (led == LED_MD)
                p = &gui_data.md;
        else
                return;
        old = *p;
        if (status == 0) {
                resetcounter[led]--;
                if (resetcounter[led] > 0)
                        return;
        }
#ifdef RETROPLATFORM
        if (led == LED_HD)
                rp_hd_activity (unitnum, status ? 1 : 0, status == 2 ? 1 : 0);
        else if (led == LED_CD)
                rp_cd_activity (unitnum, status);
#endif
        *p = status;
        resetcounter[led] = 6;
        if (old != *p)
                gui_led (led, *p);
}

void gui_flicker_led (int led, int unitnum, int status)
{
        if (led < 0) {
                gui_flicker_led2 (LED_HD, 0, 0);
                gui_flicker_led2 (LED_CD, 0, 0);
                gui_flicker_led2 (LED_MD, 0, 0);
        } else {
                gui_flicker_led2 (led, unitnum, status);
        }
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
}

void gui_gameport_button_change (int port, int button, int onoff)
{
}

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected)
{
}

void gui_filename (int num, const char *name)
{
}
