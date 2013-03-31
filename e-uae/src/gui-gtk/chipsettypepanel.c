/*
 * chipsettypepanel.c
 *
 * Part of the Gtk+ UI for E-UAE
 * Copyright 2003-2004 Richard Drummond
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "chipsettypepanel.h"
#include "chooserwidget.h"
#include "util.h"

#include "sysconfig.h"
#include "sysdeps.h"

#include "custom.h"

static void chipsettypepanel_init (ChipsetTypePanel *pathent);
static void chipsettypepanel_class_init (ChipsetTypePanelClass *class);
static void update_state (ChipsetTypePanel *panel);
static void on_chipsettype_changed (GtkWidget *w, ChipsetTypePanel *panel);
static void on_frequency_changed (GtkWidget *w, ChipsetTypePanel *panel);


guint chipsettypepanel_get_type (void)
{
    static guint chipsettypepanel_type = 0;

    if (!chipsettypepanel_type) {
	static const GtkTypeInfo chipsettypepanel_info = {
	    (char *) "ChipsetTypePanel",
	    sizeof (ChipsetTypePanel),
	    sizeof (ChipsetTypePanelClass),
	    (GtkClassInitFunc) chipsettypepanel_class_init,
	    (GtkObjectInitFunc) chipsettypepanel_init,
	    NULL,
	    NULL,
	    (GtkClassInitFunc) NULL
	};
	chipsettypepanel_type = gtk_type_unique (gtk_frame_get_type (), &chipsettypepanel_info);
    }
    return chipsettypepanel_type;
}

enum {
    CHIPSET_CHANGE_SIGNAL,
    LAST_SIGNAL
};

static guint chipsettypepanel_signals[LAST_SIGNAL];

static void chipsettypepanel_class_init (ChipsetTypePanelClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (ChipsetTypePanelClass, chipsettypepanel),
				   chipsettypepanel_signals,
				   "chipset-changed",
				   (void*)0);
    class->chipsettypepanel = NULL;
}

static void chipsettypepanel_init (ChipsetTypePanel *panel)
{
    GtkWidget *table;
    GSList *group = NULL;

    gtk_frame_set_label (GTK_FRAME (panel), "Chipset type");
    gtk_container_set_border_width (GTK_CONTAINER (panel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME (panel), 0.01, 0.5);

    gtkutil_add_table (GTK_WIDGET (panel),
	make_label ("Chipset model"), 1, 1, GTK_FILL,
	panel->chipsettype_widget = make_chooser (4, "OCS", "ECS Agnus", "Full ECS", "AGA"), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
        gtkutil_make_radio_group (group, &panel->frequency_widget[0], "NTSC", "PAL", NULL), 1, 2, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	GTKUTIL_TABLE_END
    );

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->frequency_widget[0]), TRUE);

    gtk_signal_connect (GTK_OBJECT (panel->chipsettype_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_chipsettype_changed),
			panel);
    gtk_signal_connect (GTK_OBJECT (panel->frequency_widget[0]), "clicked",
			GTK_SIGNAL_FUNC (on_frequency_changed),
			panel);
    gtk_signal_connect (GTK_OBJECT (panel->frequency_widget[0]), "clicked",
			GTK_SIGNAL_FUNC (on_frequency_changed),
			panel);
}


static void on_chipsettype_changed (GtkWidget *w, ChipsetTypePanel *panel)
{

    guint choice = CHOOSERWIDGET (w)->choice;

    guint chipset_mask = 0;

    if (choice > 0)
        chipset_mask |= CSMASK_ECS_AGNUS;
    if (choice > 1)
        chipset_mask |= CSMASK_ECS_DENISE;
#ifdef AGA
    if (choice > 2)
        chipset_mask |= CSMASK_AGA;
#endif
   
    panel->chipset_mask = chipset_mask;

    gtk_signal_emit_by_name (GTK_OBJECT(panel), "chipset-changed");
}

static void on_frequency_changed (GtkWidget *w, ChipsetTypePanel *panel)
{
     panel->ntscmode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (panel->frequency_widget[0]));
   
     gtk_signal_emit_by_name (GTK_OBJECT(panel), "chipset-changed");    
}

GtkWidget *chipsettypepanel_new (void)
{
    ChipsetTypePanel *w = CHIPSETTYPEPANEL (gtk_type_new (chipsettypepanel_get_type ()));

    return GTK_WIDGET (w);
}

void chipsettypepanel_set_chipset_mask (ChipsetTypePanel *panel, guint chipset_mask)
{
    int choice = 0;
   
    if (chipset_mask & CSMASK_ECS_DENISE)
        choice = 2;
    if (chipset_mask & CSMASK_ECS_AGNUS)
        choice = 1;
#ifdef AGA
    if (chipset_mask & CSMASK_AGA)
        choice = 3;
#endif
    chooserwidget_set_choice (CHOOSERWIDGET (panel->chipsettype_widget), choice);
}

void chipsettypepanel_set_ntscmode (ChipsetTypePanel *panel, guint ntscmode)
{
   int buttonno = ntscmode == FALSE ? 1 : 0;
   
   gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->frequency_widget[buttonno]), TRUE);
}
