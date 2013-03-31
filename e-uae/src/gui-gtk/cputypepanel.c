/*
 * cpuspeedpanel.c
 *
 * Copyright 2003-2004 Richard Drummond
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "cputypepanel.h"
#include "chooserwidget.h"
#include "util.h"

static void cputypepanel_init (CpuTypePanel *pathent);
static void cputypepanel_class_init (CpuTypePanelClass *class);
static void update_state (CpuTypePanel *ctpanel);
static void on_cputype_changed (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_addr24bit_toggled (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_fpuenabled_toggled (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_accuracy_changed (GtkWidget *w, CpuTypePanel *ctpanel);

guint cputypepanel_get_type ()
{
    static guint cputypepanel_type = 0;

    if (!cputypepanel_type) {
	static const GtkTypeInfo cputypepanel_info = {
	    (char *) "CpuTypePanel",
	    sizeof (CpuTypePanel),
	    sizeof (CpuTypePanelClass),
	    (GtkClassInitFunc) cputypepanel_class_init,
	    (GtkObjectInitFunc) cputypepanel_init,
	    NULL,
	    NULL,
	    (GtkClassInitFunc) NULL
	};
	cputypepanel_type = gtk_type_unique (gtk_frame_get_type (), &cputypepanel_info);
    }
    return cputypepanel_type;
}

enum {
    TYPE_CHANGE_SIGNAL,
    ADDR24_CHANGE_SIGNAL,
    LAST_SIGNAL
};

static guint cputypepanel_signals[LAST_SIGNAL];

static void cputypepanel_class_init (CpuTypePanelClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (CpuTypePanelClass, cputypepanel),
				   cputypepanel_signals,
				   "cputype-changed",
				   "addr24bit-changed",
				   (void*)0);
    class->cputypepanel = NULL;
}

static void cputypepanel_init (CpuTypePanel *ctpanel)
{
    GtkWidget *table;

    gtk_frame_set_label (GTK_FRAME(ctpanel), "CPU Emulation");
    gtk_container_set_border_width (GTK_CONTAINER (ctpanel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME(ctpanel), 0.01, 0.5);

    gtkutil_add_table (GTK_WIDGET (ctpanel),
	make_label ("CPU Model"), 1, 1, GTK_FILL,
	ctpanel->cputype_widget    = make_chooser (5, "68000", "68010", "68020", "68040", "68060"), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	ctpanel->addr24bit_widget  = gtk_check_button_new_with_label ("24-bit addressing"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	ctpanel->fpuenabled_widget = gtk_check_button_new_with_label ("Emulate FPU"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	make_label ("Accuracy"), 1, 1, GTK_FILL,
	ctpanel->accuracy_widget   = make_chooser (3, "Normal", "Compatible", "Cycle exact"), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	GTKUTIL_TABLE_END
    );

    gtk_signal_connect (GTK_OBJECT (ctpanel->cputype_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_cputype_changed),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->addr24bit_widget), "toggled",
			GTK_SIGNAL_FUNC (on_addr24bit_toggled),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->fpuenabled_widget), "toggled",
			GTK_SIGNAL_FUNC (on_fpuenabled_toggled),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->accuracy_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_accuracy_changed),
			ctpanel);

    update_state (ctpanel);
}

static void update_state (CpuTypePanel *ctpanel)
{
    guint cpu    = ctpanel->cputype;
    guint addr24 = ctpanel->addr24bit;
    guint fpu    = ctpanel->fpuenabled;

    switch (cpu) {
	case 0:
	case 1:
	    addr24 = 1;
	    fpu    = 0;
	    break;
	case 3:
	case 4:
	    addr24 = 0;
	    fpu    = 1;
	    break;
    }

    gtk_widget_set_sensitive (ctpanel->addr24bit_widget, cpu == 2);
    gtk_widget_set_sensitive (ctpanel->fpuenabled_widget, cpu == 2);

    if (fpu != ctpanel->fpuenabled) {
        ctpanel->fpuenabled = fpu;
        gtk_signal_handler_block_by_data (GTK_OBJECT (ctpanel->fpuenabled_widget), ctpanel );
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ctpanel->fpuenabled_widget), fpu);
        gtk_signal_handler_unblock_by_data (GTK_OBJECT (ctpanel->fpuenabled_widget), ctpanel );
    }

    if (addr24 != ctpanel->addr24bit) {
	ctpanel->addr24bit  = addr24;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ctpanel->addr24bit_widget), addr24);
    }

    gtk_widget_set_sensitive (ctpanel->accuracy_widget, cpu == 0);
    if (cpu > 0)
	chooserwidget_set_choice (CHOOSERWIDGET (ctpanel->accuracy_widget), 0);
}

static void on_cputype_changed (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->cputype = CHOOSERWIDGET (w)->choice;
    update_state (ctpanel);
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "cputype-changed");
}

static void on_addr24bit_toggled (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->addr24bit = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ctpanel->addr24bit_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "addr24bit-changed");
}

static void on_fpuenabled_toggled (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->fpuenabled = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ctpanel->fpuenabled_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "cputype-changed");
}

static void on_accuracy_changed (GtkWidget *w, CpuTypePanel *ctpanel)
{
    int choice = CHOOSERWIDGET (ctpanel->accuracy_widget)->choice;

    if (choice == 0 ) {
	ctpanel->compatible = 0;
	ctpanel->cycleexact = 0;
    } else if (choice == 1) {
	ctpanel->compatible = 1;
	ctpanel->cycleexact = 0;
    } else {
	ctpanel->compatible = 0;
	ctpanel->cycleexact = 1;
    }

    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "cputype-changed");
}

GtkWidget *cputypepanel_new (void)
{
    CpuTypePanel *w = CPUTYPEPANEL (gtk_type_new (cputypepanel_get_type ()));

    return GTK_WIDGET (w);
}

void cputypepanel_set_cpulevel (CpuTypePanel *ctpanel, guint cpulevel)
{
    guint cputype; guint fpu = ctpanel->fpuenabled;

    switch (cpulevel) {
	case 0:  cputype = 0; break;
	case 1:  cputype = 1; break;
	case 2:  cputype = 2; fpu = 0; break;
	case 3:  cputype = 2; fpu = 1; break;
	case 4:  cputype = 3; fpu = 1; break;
	case 6:  cputype = 4; fpu = 1; break;
	default: cputype = 0;
    }
    if (cputype != ctpanel->cputype) {
	ctpanel->cputype = cputype;
	chooserwidget_set_choice (CHOOSERWIDGET (ctpanel->cputype_widget), cputype);

    }
    if (fpu != ctpanel->fpuenabled) {
	ctpanel->fpuenabled = fpu;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ctpanel->fpuenabled_widget), fpu);
    }
    update_state (ctpanel);
}

guint cputypepanel_get_cpulevel (CpuTypePanel *ctpanel)
{
    int cpulevel;

    switch (ctpanel->cputype) {
	case 0:  cpulevel = 0; break;
	case 1:  cpulevel = 1; break;
	case 2:  cpulevel = 2 + (ctpanel->fpuenabled!=0); break;
	case 3:  cpulevel = 4 ; break;
	case 4:
	default: cpulevel = 6; break;
    }

    return cpulevel;
}

guint cputypepanel_get_addr24bit (CpuTypePanel *ctpanel)
{
    return ctpanel->addr24bit;
}

void cputypepanel_set_addr24bit (CpuTypePanel *ctpanel, guint addr24bit)
{
    if (ctpanel->cputype == 2)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
				     (ctpanel->addr24bit_widget),
				      addr24bit);
}

void cputypepanel_set_compatible (CpuTypePanel *ctpanel, gboolean compatible)
{
    chooserwidget_set_choice (CHOOSERWIDGET (ctpanel->accuracy_widget),
			      compatible ? 1 : 0);
}

void cputypepanel_set_cycleexact (CpuTypePanel *ctpanel, gboolean cycleexact)
{
    chooserwidget_set_choice (CHOOSERWIDGET (ctpanel->accuracy_widget),
			      cycleexact ? 2 : 0);
}
