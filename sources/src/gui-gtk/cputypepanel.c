/*
 * cpuspeedpanel.c
 *
 * Copyright 2003-2004 Richard Drummond
 */

#include "sysdeps.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "gcc_warnings.h"
GCC_DIAG_OFF(strict-prototypes)
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
GCC_DIAG_ON(strict-prototypes)

#include "cputypepanel.h"
#include "chooserwidget.h"
#include "util.h"

static void cputypepanel_init (CpuTypePanel *pathent);
static void cputypepanel_class_init (CpuTypePanelClass *class);
static void fputypepanel_init (FpuTypePanel *pathent);
static void fputypepanel_class_init (FpuTypePanelClass *class);
static void update_state_cpu (CpuTypePanel *ctpanel);
static void update_state_fpu (FpuTypePanel *ftpanel);
static void on_cputype_changed (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_addr24bit_toggled (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_cpu_morecompat_changed (GtkWidget *w, CpuTypePanel *ctpanel);
static void on_mmu40_changed (GtkWidget *w, CpuTypePanel *ctpanel);

static void on_fputype_changed (GtkWidget *w, FpuTypePanel *ftpanel);
static void on_fpu_morecompat_changed (GtkWidget *w, FpuTypePanel *ftpanel);

GtkType cputypepanel_get_type ()
{
	static bool    hasCputype = false;
    static GtkType cputypepanel_type;

    if (!hasCputype) {
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
		hasCputype = true;
    }
    return cputypepanel_type;
}

guint fputypepanel_get_type ()
{
	static guint fputypepanel_type = 0;

	if (!fputypepanel_type) {
	static const GtkTypeInfo fputypepanel_info = {
		(char *) "FpuTypePanel",
		sizeof (FpuTypePanel),
		sizeof (FpuTypePanelClass),
		(GtkClassInitFunc) fputypepanel_class_init,
		(GtkObjectInitFunc) fputypepanel_init,
		NULL,
		NULL,
		(GtkClassInitFunc) NULL
	};
	fputypepanel_type = gtk_type_unique (gtk_frame_get_type (), &fputypepanel_info);
	}
	return fputypepanel_type;
}

enum {
    TYPE_CHANGE_SIGNAL,
    ADDR24_CHANGE_SIGNAL,
    LAST_SIGNAL
};

static guint cputypepanel_signals[LAST_SIGNAL];
static guint fputypepanel_signals[LAST_SIGNAL];

static void cputypepanel_class_init (CpuTypePanelClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (CpuTypePanelClass, cputypepanel),
				   cputypepanel_signals,
				   "cputype-changed",
				   "addr24bit-changed",
				   "cpucompat-changed",
				   "mmu40-changed",
				   (void*)0);
    class->cputypepanel = NULL;
}

static void fputypepanel_class_init (FpuTypePanelClass *class)
{
    gtkutil_add_signals_to_class ((GtkObjectClass *)class,
				   GTK_STRUCT_OFFSET (FpuTypePanelClass, fputypepanel),
				   fputypepanel_signals,
				   "fputype-changed",
				   "fpucompat-changed",
				   (void*)0);
    class->fputypepanel = NULL;
}

static void cputypepanel_init (CpuTypePanel *ctpanel)
{
    GtkWidget *table;

    gtk_frame_set_label (GTK_FRAME(ctpanel), "CPU Emulation");
    gtk_container_set_border_width (GTK_CONTAINER (ctpanel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME(ctpanel), 0.01, 0.5);

    gtkutil_add_table (GTK_WIDGET (ctpanel),
	make_label ("CPU Model"), 1, 1, GTK_FILL,
	ctpanel->cputype_widget    = make_chooser ( 6, "68000", "68010", "68020", "68030", "68040", "68060" ), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	ctpanel->addr24bit_widget  = gtk_check_button_new_with_label ("24-bit addressing"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	ctpanel->cpu_morecompat_widget  = gtk_check_button_new_with_label ("More Compatible"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	ctpanel->mmu40_widget  = gtk_check_button_new_with_label ("68040 MMU"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	GTKUTIL_TABLE_END
    );

    gtk_signal_connect (GTK_OBJECT (ctpanel->cputype_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_cputype_changed),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->addr24bit_widget), "toggled",
			GTK_SIGNAL_FUNC (on_addr24bit_toggled),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->cpu_morecompat_widget), "toggled",
			GTK_SIGNAL_FUNC (on_cpu_morecompat_changed),
			ctpanel);
    gtk_signal_connect (GTK_OBJECT (ctpanel->mmu40_widget), "toggled",
			GTK_SIGNAL_FUNC (on_mmu40_changed),
			ctpanel);

    update_state_cpu (ctpanel);
}

static void fputypepanel_init (FpuTypePanel *ftpanel)
{
    GtkWidget *table;

    gtk_frame_set_label (GTK_FRAME(ftpanel), "FPU Emulation");
    gtk_container_set_border_width (GTK_CONTAINER (ftpanel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME(ftpanel), 0.01, 0.5);

    gtkutil_add_table (GTK_WIDGET (ftpanel),
	make_label ("FPU Model"), 1, 1, GTK_FILL,

	ftpanel->fputype_widget    = make_chooser (
#ifdef FPUEMU
						   4, "None", "68881", "68882", "CPU Internal"
#else
						   1, "None"
#endif
						   ), 2, 1, GTK_EXPAND | GTK_FILL,
	GTKUTIL_ROW_END,
	ftpanel->fpu_morecompat_widget  = gtk_check_button_new_with_label ("More Compatible"), 1, 2, GTK_EXPAND,
	GTKUTIL_ROW_END,
	GTKUTIL_TABLE_END
    );

    gtk_signal_connect (GTK_OBJECT (ftpanel->fputype_widget), "selection-changed",
			GTK_SIGNAL_FUNC (on_fputype_changed),
			ftpanel);
    gtk_signal_connect (GTK_OBJECT (ftpanel->fpu_morecompat_widget), "toggled",
			GTK_SIGNAL_FUNC (on_fpu_morecompat_changed),
			ftpanel);

    update_state_fpu (ftpanel);
}

static void update_state_cpu (CpuTypePanel *ctpanel)
{
	guint cpu			= ctpanel->cputype;
	guint addr24		= ctpanel->addr24bit;
	guint cpumorecompat	= ctpanel->cpumorecompat;

    switch (cpu) {
		case 0:
		case 1:
		    addr24 = 1;
		    break;
		case 3:
		case 4:
		    addr24 = 0;
		    break;
    }

    gtk_widget_set_sensitive (ctpanel->addr24bit_widget, cpu == 2);

    if (addr24 != ctpanel->addr24bit) {
	ctpanel->addr24bit  = addr24;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ctpanel->addr24bit_widget), addr24);
    }
}

static void update_state_fpu (FpuTypePanel *ftpanel)
{

}

static void on_cputype_changed (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->cputype = CHOOSERWIDGET (w)->choice;
    update_state_cpu (ctpanel);
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "cputype-changed");
}

static void on_addr24bit_toggled (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->addr24bit = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ctpanel->addr24bit_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "addr24bit-changed");
}

static void on_cpu_morecompat_changed (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->cpumorecompat = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ctpanel->cpu_morecompat_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "cpucompat-changed");
}

static void on_mmu40_changed (GtkWidget *w, CpuTypePanel *ctpanel)
{
    ctpanel->mmu40 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ctpanel->mmu40_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ctpanel), "mmu40-changed");
}

static void on_fputype_changed (GtkWidget *w, FpuTypePanel *ftpanel)
{
	ftpanel->fputype = CHOOSERWIDGET (w)->choice;
	update_state_fpu (ftpanel);
	gtk_signal_emit_by_name (GTK_OBJECT(ftpanel), "fputype-changed");
}

static void on_fpu_morecompat_changed (GtkWidget *w, FpuTypePanel *ftpanel)
{
    ftpanel->fpumorecompat = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ftpanel->fpu_morecompat_widget));
    gtk_signal_emit_by_name (GTK_OBJECT(ftpanel), "fpucompat-changed");
}

GtkWidget *cputypepanel_new (void)
{
    CpuTypePanel *w = CPUTYPEPANEL (gtk_type_new (cputypepanel_get_type ()));

    return GTK_WIDGET (w);
}

GtkWidget *fputypepanel_new (void)
{
    CpuTypePanel *w = CPUTYPEPANEL (gtk_type_new (fputypepanel_get_type ()));

    return GTK_WIDGET (w);
}

void cputypepanel_set_cpulevel (CpuTypePanel *ctpanel, guint cpulevel)
{
    guint cputype;
	guint fpu = ctpanel->fpuenabled;

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
    update_state_cpu (ctpanel);
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
