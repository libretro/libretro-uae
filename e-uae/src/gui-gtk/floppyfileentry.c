/*
 * floppyfileentry.c
 *
 * Copyright 2004 Martin Garton
 * Copyright 2004 Richard Drummond
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "util.h"
#include "floppyfileentry.h"
#include "led.h"

static void floppyfileentry_init (FloppyFileEntry *ffe);
static void floppyfileentry_class_init (FloppyFileEntryClass *class);
static void on_eject (GtkWidget *w, FloppyFileEntry *ffe);
static void on_insert (GtkWidget *w, FloppyFileEntry *ffe);

guint floppyfileentry_get_type ()
{
    static guint floppyfileentry_type = 0;

    if (!floppyfileentry_type) {
	static const GtkTypeInfo floppyfileentry_info = {
	    (char *) "FloppyFileEntry",
	    sizeof (FloppyFileEntry),
	    sizeof (FloppyFileEntryClass),
	    (GtkClassInitFunc) floppyfileentry_class_init,
	    (GtkObjectInitFunc) floppyfileentry_init,
	    NULL,
	    NULL,
	    (GtkClassInitFunc) NULL
	};
	floppyfileentry_type = gtk_type_unique (gtk_frame_get_type (), &floppyfileentry_info);
    }
    return floppyfileentry_type;
}

enum {
    DISC_CHANGE_SIGNAL,
    LAST_SIGNAL
};

static guint floppyfileentry_signals[LAST_SIGNAL];

static void floppyfileentry_class_init (FloppyFileEntryClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (FloppyFileEntryClass, floppyfileentry),
				   floppyfileentry_signals,
				   "disc-changed",
				   (void *)0);

    class->floppyfileentry = NULL;
}

static gchar *ffe_currentdir;

static void floppyfileentry_init (FloppyFileEntry *ffe)
{
    GtkWidget *hbox;
    GtkWidget *ledbox;
    GtkWidget *bbox;

    gtk_container_set_border_width (GTK_CONTAINER (ffe), 8);
    gtk_frame_set_label_align (GTK_FRAME (ffe), 0.01, 0.5);

    ffe->led = led_new ();
    ledbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (ledbox), gtk_hbox_new (FALSE, 0));
    gtk_container_add (GTK_CONTAINER (ledbox), ffe->led);
    gtk_container_add (GTK_CONTAINER (ledbox), gtk_hbox_new (FALSE, 0));

    ffe->path_widget = gtk_entry_new ();
    GTK_WIDGET_UNSET_FLAGS (ffe->path_widget, GTK_CAN_FOCUS);
    gtk_editable_set_editable (GTK_EDITABLE (ffe->path_widget), FALSE);

    bbox = gtk_hbox_new (TRUE, 2);
    ffe->insert_button = gtk_button_new_with_label ("Insert...");
    ffe->eject_button  = gtk_button_new_with_label ("Eject");
    gtk_widget_set_sensitive (ffe->eject_button, FALSE);
    gtk_box_pack_start (GTK_BOX (bbox), ffe->eject_button, TRUE, TRUE, 2);
    gtk_box_pack_start (GTK_BOX (bbox), ffe->insert_button, TRUE, TRUE, 2);

    hbox = gtk_hbox_new (FALSE, 3);
    gtk_box_pack_start (GTK_BOX (hbox), ledbox, FALSE, FALSE, 2);
    gtk_box_pack_start (GTK_BOX (hbox), ffe->path_widget, TRUE, TRUE, 2);
    gtk_box_pack_start (GTK_BOX (hbox), bbox, FALSE, FALSE, 2);

    gtk_signal_connect (GTK_OBJECT (ffe->eject_button), "clicked",
			GTK_SIGNAL_FUNC (on_eject),
			ffe);

    gtk_container_add (GTK_CONTAINER (ffe), hbox);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
    gtk_widget_show_all (hbox);

    gtk_signal_connect (GTK_OBJECT (ffe->insert_button), "clicked",
			GTK_SIGNAL_FUNC (on_insert),
			ffe);
}


static void on_eject (GtkWidget *w, FloppyFileEntry *ffe)
{
    if (ffe->filename)
	g_free(ffe->filename);
    ffe->filename = NULL;
    gtk_entry_set_text (GTK_ENTRY (ffe->path_widget), "");
    gtk_widget_set_sensitive (ffe->eject_button, 0);

    gtk_signal_emit_by_name (GTK_OBJECT (ffe), "disc-changed");
}

static void on_filesel_close (FloppyFileEntry *ffe, gpointer data)
{
    gtk_widget_set_sensitive (ffe->insert_button, 1);
    if (ffe->filename && strlen(ffe->filename))
	gtk_widget_set_sensitive (ffe->eject_button, 1);

    gtk_widget_destroy (ffe->filesel);
}

static void on_filesel_ok (FloppyFileEntry *ffe, gpointer data)
{
    const gchar *fname = gtk_file_selection_get_filename (GTK_FILE_SELECTION (ffe->filesel));

    if (fname && strlen (fname)) {
	struct stat statbuf;

	if (stat (fname, &statbuf) == 0) {
	    /* Path fname exists */

	    if (ffe_currentdir) {
		g_free (ffe_currentdir);
		ffe_currentdir = 0;
	    }

	    if (S_ISDIR (statbuf.st_mode)) {
		/* But it's a directory. Make sure we
		 * have a trailing path separator
		 */
	        int len = strlen (fname);
		if (fname[len-1] != '/')
		    ffe_currentdir = g_strconcat (fname, "/", NULL);
		else
		    ffe_currentdir = g_strdup (fname);
	    } else {
		/* Yay! It's a file */
		const gchar *p = strrchr (fname, '/');

		/* Free old file path */
        	if (ffe->filename) {
		    g_free (ffe->filename);
		    ffe->filename = 0;
		}

		if (p)
		    ffe_currentdir = g_strndup (fname, p - fname + 1);
		else
		    ffe_currentdir = g_strdup (fname);

		ffe->filename = g_strdup (fname);

		gtk_entry_set_text (GTK_ENTRY (ffe->path_widget), ffe->filename);
		gtk_signal_emit_by_name (GTK_OBJECT(ffe), "disc-changed");
	    }
	}
    }
    gtk_widget_destroy (ffe->filesel);

    gtk_widget_set_sensitive (ffe->insert_button, 1);
    if (ffe->filename && strlen(ffe->filename))
	gtk_widget_set_sensitive (ffe->eject_button, 1);
}

static void on_insert (GtkWidget *w, FloppyFileEntry *ffe)
{
    gchar *title;

    gtk_widget_set_sensitive (ffe->insert_button, 0);
    gtk_widget_set_sensitive (ffe->eject_button, 0);

    title = g_strconcat ("Select floppy image to insert in drive ", ffe->drivename, NULL);

    ffe->filesel = gtk_file_selection_new (title);

    if (ffe->filesel) {
        gtk_file_selection_set_filename (GTK_FILE_SELECTION (ffe->filesel),
					 ffe_currentdir ? ffe_currentdir : "");

	gtk_signal_connect_object (GTK_OBJECT (ffe->filesel),
				   "destroy",
				   (GtkSignalFunc) on_filesel_close,
				   (gpointer)ffe);
	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (ffe->filesel)->ok_button),
				   "clicked",
				   (GtkSignalFunc) on_filesel_ok,
				   (gpointer)ffe);

	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (ffe->filesel)->cancel_button),
				   "clicked",
				   (GtkSignalFunc) on_filesel_close,
				   GTK_OBJECT (ffe));
	gtk_widget_show (ffe->filesel);
    } else {
	gtk_widget_set_sensitive (ffe->insert_button, 1);
	gtk_widget_set_sensitive (ffe->eject_button, 1);
    }

    if (title)
	g_free (title);
}

GtkWidget *floppyfileentry_new (void)
{
    FloppyFileEntry *w = FLOPPYFILEENTRY (gtk_type_new (floppyfileentry_get_type ()));

    return GTK_WIDGET (w);
}

void floppyfileentry_set_drivename (FloppyFileEntry *ffe, const gchar *drivename)
{
    if (ffe->drivename)
	g_free (ffe->drivename);
    ffe->drivename = g_strdup (drivename);
}

void floppyfileentry_set_currentdir (FloppyFileEntry *ffe, const gchar *pathname)
{
    int len = strlen (pathname);

    if (ffe_currentdir)
	g_free (ffe_currentdir);

    /*
     * Make sure it has a trailing path separator so the file dialog
     * actually believes it's a directory
     */
    ffe_currentdir = g_strconcat ((gchar *)pathname,
				  (pathname[len-1] != '/') ? "/" : NULL,
				   NULL);
}

void floppyfileentry_set_filename (FloppyFileEntry *ffe, const gchar *filename)
{
    gtk_entry_set_text (GTK_ENTRY (ffe->path_widget), filename);

    if (ffe->filename) {
	g_free (ffe->filename);
	ffe->filename = 0;
    }

    if (filename && strlen (filename)) {
	ffe->filename = g_strdup (filename);
	gtk_widget_set_sensitive (ffe->eject_button, 1);
    } else
	gtk_widget_set_sensitive (ffe->eject_button, 0);
}

#if GTK_MAJOR_VERSION > 2
G_CONST_RETURN
#endif
gchar *floppyfileentry_get_filename (FloppyFileEntry *ffe)
{
    return ffe->filename;
}

void floppyfileentry_set_label (FloppyFileEntry *ffe, const gchar *label)
{
    gtk_frame_set_label (GTK_FRAME (ffe), label);
}

void floppyfileentry_set_led (FloppyFileEntry *ffe, gboolean state)
{
    GdkColor color;

    if (state)
	color = (GdkColor){0, 0x0000, 0xFFFF, 0x0000};
    else
	color = (GdkColor){0, 0x0000, 0x0000, 0x0000};

    led_set_color (LED (ffe->led), color);
}

void floppyfileentry_do_dialog (FloppyFileEntry *ffe)
{
    on_insert (NULL, ffe);
}
