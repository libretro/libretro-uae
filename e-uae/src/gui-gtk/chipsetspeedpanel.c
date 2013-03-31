/*
 * chipsetspeedpanel.c
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

#include "chipsetspeedpanel.h"
#include "chooserwidget.h"
#include "util.h"

#include "sysconfig.h"
#include "sysdeps.h"

#include "custom.h"

static void chipsetspeedpanel_init (ChipsetSpeedPanel *pathent);
static void chipsetspeedpanel_class_init (ChipsetSpeedPanelClass *class);
static void update_state (ChipsetSpeedPanel *panel);

static void on_framerate_changed (GtkWidget *w, ChipsetSpeedPanel *panel);
static void on_sprite_collisions_changed (GtkWidget *w, ChipsetSpeedPanel *panel);
static void on_immediate_blits_changed (GtkWidget *w, ChipsetSpeedPanel *panel);

guint chipsetspeedpanel_get_type (void)
{
    static guint chipsetspeedpanel_type = 0;

    if (!chipsetspeedpanel_type) {
	static const GtkTypeInfo chipsetspeedpanel_info = {
	    (char *) "ChipsetSpeedPanel",
	    sizeof (ChipsetSpeedPanel),
	    sizeof (ChipsetSpeedPanelClass),
	    (GtkClassInitFunc) chipsetspeedpanel_class_init,
	    (GtkObjectInitFunc) chipsetspeedpanel_init,
	    NULL,
	    NULL,
	    (GtkClassInitFunc) NULL
	};
	chipsetspeedpanel_type = gtk_type_unique (gtk_frame_get_type (), &chipsetspeedpanel_info);
    }
    return chipsetspeedpanel_type;
}

enum {
    FRAMERATE_CHANGE_SIGNAL,
    SPRITE_COLLISION_CHANGE_SIGNAL,
    IMMEDIATE_BLITS_CHANGE_SIGNAL,
    LAST_SIGNAL
};

static guint chipsetspeedpanel_signals[LAST_SIGNAL];

static void chipsetspeedpanel_class_init (ChipsetSpeedPanelClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (ChipsetSpeedPanelClass, chipsetspeedpanel),
				   chipsetspeedpanel_signals,
				   "framerate-changed",
				   "sprite-collisions-changed",
				   "immediate-blits-changed",
				   (void*)0);
    class->chipsetspeedpanel = NULL;
}

static void chipsetspeedpanel_init (ChipsetSpeedPanel *panel)
{
    GtkWidget *table;
    GSList *group = NULL;

    gtk_frame_set_label (GTK_FRAME (panel), "Emulation speed");
    gtk_container_set_border_width (GTK_CONTAINER (panel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME (panel), 0.01, 0.5);

    gtkutil_add_table (GTK_WIDGET (panel),
        make_label("Draw one\nframe in"), 1, 1, GTK_FILL,
        panel->framerate_widget = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 1, 21, 1, 1, 1))), 2, 1, GTK_FILL,
        GTKUTIL_ROW_END,
	make_label ("Sprite collisions"), 1, 1, GTK_FILL,
	panel->collisions_widget = make_chooser (4, "None", "Sprites only", "Sprites & playfields", "Full"), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	panel->immediate_blits_widget = gtk_check_button_new_with_label ("Immediate blits"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	GTKUTIL_TABLE_END
    );
    gtk_scale_set_value_pos (GTK_SCALE (panel->framerate_widget), GTK_POS_TOP);
    gtk_scale_set_digits (GTK_SCALE (panel->framerate_widget), 0);

    gtk_signal_connect (GTK_OBJECT ( GTK_RANGE(panel->framerate_widget)->adjustment), "value-changed",
			GTK_SIGNAL_FUNC (on_framerate_changed),
			panel);
    gtk_signal_connect (GTK_OBJECT (panel->collisions_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_sprite_collisions_changed),
			panel);
    gtk_signal_connect (GTK_OBJECT (panel->immediate_blits_widget), "toggled",
			GTK_SIGNAL_FUNC (on_immediate_blits_changed),
			panel);
}

static void on_framerate_changed (GtkWidget *w, ChipsetSpeedPanel *panel)
{
    panel->framerate = (guint) GTK_ADJUSTMENT (GTK_RANGE (panel->framerate_widget)->adjustment)->value;
    gtk_signal_emit_by_name (GTK_OBJECT (panel), "framerate-changed");
}

static void on_sprite_collisions_changed (GtkWidget *w, ChipsetSpeedPanel *panel)
{
    panel->collision_level = CHOOSERWIDGET (w)->choice;
    gtk_signal_emit_by_name (GTK_OBJECT (panel), "sprite-collisions-changed");
}

static void on_immediate_blits_changed (GtkWidget *w, ChipsetSpeedPanel *panel)
{
     panel->immediate_blits = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (panel->immediate_blits_widget));
     gtk_signal_emit_by_name (GTK_OBJECT(panel), "immediate-blits-changed");
}


GtkWidget *chipsetspeedpanel_new (void)
{
    ChipsetSpeedPanel *w = CHIPSETSPEEDPANEL (gtk_type_new (chipsetspeedpanel_get_type ()));

    return GTK_WIDGET (w);
}

void chipsetspeedpanel_set_framerate (ChipsetSpeedPanel *panel, guint framerate)
{
    gtk_adjustment_set_value (GTK_ADJUSTMENT (GTK_RANGE (panel->framerate_widget)->adjustment), framerate);
}

void chipsetspeedpanel_set_collision_level (ChipsetSpeedPanel *panel, guint collision_level)
{
    chooserwidget_set_choice (CHOOSERWIDGET (panel->collisions_widget), collision_level);
}

void chipsetspeedpanel_set_immediate_blits (ChipsetSpeedPanel *panel, guint immediate_blits)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->immediate_blits_widget), immediate_blits != 0);
}
