/*
 * util.c
 *
 * Copyright 2003-2004 Richard Drummond
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "util.h"
#include "chooserwidget.h"

/*
 * Some utility functions to make building a GTK+ GUI easier
 * and more compact, to hide differences between GTK1.x and GTK2.x
 * and to help maintain consistency
 */

/*
 * Create a list of signals and add them to a GTK+ class
 */
void gtkutil_add_signals_to_class (GtkObjectClass *class, guint func_offset, guint *signals, ...)
{
   va_list signames;
   const char *name;
   unsigned int count = 0;

   va_start (signames, signals);
   name = va_arg (signames, const char *);

   while (name) {
#if GTK_MAJOR_VERSION >= 2
	signals[count] = g_signal_new (name, G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
				func_offset, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
#else
	signals[count] = gtk_signal_new (name, GTK_RUN_FIRST, class->type, func_offset,
					gtk_signal_default_marshaller, GTK_TYPE_NONE, 0);
#endif
	count++;
	name = va_arg (signames, const char *);
   };

#if GTK_MAJOR_VERSION < 2
    gtk_object_class_add_signals (class, signals, count);
#endif
}


/*
 * Make a simple chooser from a combo widget
 * <count> is the number of choices, and the choices
 * themselves are supplied as a list of strings.
 */
GtkWidget *make_chooser (int count, ...)
{
    GtkWidget *chooser;
    GList     *list = 0;
    va_list   choices;
    int       i;

    chooser = chooserwidget_new ();

    va_start (choices, count);
    for (i=0; i<count; i++)
	list = g_list_append (list, (gpointer) va_arg (choices, char *));
    gtk_combo_set_popdown_strings (GTK_COMBO (chooser), list);
    g_list_free (list);

    gtk_widget_show (chooser);
    return chooser;
}

GtkWidget *make_label (const char *string)
{
    GtkWidget *label;

    label = gtk_label_new (string);
    gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
    gtk_widget_show (label);

    return label;
}

#if GTK_MAJOR_VERSION < 2
GtkWidget *make_labelled_button (const char *label, GtkAccelGroup *accel_group)
{
    GtkWidget *button = gtk_button_new ();
    int key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (button)->child), "_Okay");
    gtk_widget_add_accelerator (button, "clicked", accel_group,key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
}
#endif

GtkWidget *make_panel (const char *name)
{
    GtkWidget *panel;

    panel = gtk_frame_new (name);
    gtk_widget_show (panel);
    gtk_container_set_border_width (GTK_CONTAINER (panel), PANEL_BORDER_WIDTH);
    gtk_frame_set_label_align (GTK_FRAME (panel), 0.01, 0.5);

    return panel;
}

GtkWidget *make_xtable( int width, int height )
{
    GtkWidget *table;

    table = gtk_table_new (height,width, FALSE);
    gtk_widget_show (table);

    gtk_container_set_border_width (GTK_CONTAINER (table), TABLE_BORDER_WIDTH);
    gtk_table_set_row_spacings (GTK_TABLE (table), TABLE_ROW_SPACING);
    gtk_table_set_col_spacings (GTK_TABLE (table), TABLE_COL_SPACING);

    return table;
}

/*
 * Add some padding to a vbox or hbox which will consume
 * space when the box is resized to larger than default size
 */
void add_box_padding (GtkWidget *box)
{
    GtkWidget *vbox;

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);
    gtk_box_pack_start (GTK_BOX (box), vbox, TRUE, TRUE, 0);
}

/*
 * Add some padding to a table which will consime space when
 * when the table is resized to larger than default size
 */
void add_table_padding (GtkWidget *table, int x, int y)
{
    GtkWidget *vbox;
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);
    gtk_table_attach (GTK_TABLE (table), vbox, x, x+1, y, y+1,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
}

/*
 * Add a widget to a table with some sensible defaults
 *
 * <x,y> is the table position to insert the widget
 * <width> is the the number of columns the widget will take up
 * <xflags> are the attachment flags that will apply horizontally
 */
void add_to_table(GtkWidget *table, GtkWidget *widget, int x, int y, int width, int xflags)
{
  gtk_table_attach (GTK_TABLE (table), widget, x, x+width, y, y+1,
		   (GtkAttachOptions) (xflags),
		   (GtkAttachOptions) (0), 0, 0);
}


/*
 * Super-duper, handy table creation tool!
 *
 * Creates a table and add a list of widgets to it.
 */
GtkWidget *gtkutil_add_table (GtkWidget *container, ...)
{
    va_list contents;
    GtkWidget *widget;
    GtkWidget *table;
    int row, max_col;
    int col, width;
    int flags;

    table = gtk_table_new (3, 3, FALSE);
    gtk_container_set_border_width (GTK_CONTAINER (table), TABLE_BORDER_WIDTH);
    gtk_table_set_row_spacings (GTK_TABLE (table), TABLE_ROW_SPACING);
    gtk_table_set_col_spacings (GTK_TABLE (table), TABLE_COL_SPACING);
    gtk_container_add (GTK_CONTAINER (container), table);

    va_start (contents, container);
    widget = va_arg (contents, GtkWidget *);
    row = 1;
    max_col = 1;

    while (widget != GTKUTIL_TABLE_END) {
	if (widget == GTKUTIL_ROW_END) {
	    row += 2;
	} else {
	    col   = va_arg (contents, gint);
	    if (col > max_col) max_col = col;
	    width = va_arg (contents, gint);
	    flags = va_arg (contents, gint);

	    gtk_table_attach (GTK_TABLE (table), widget, col, col+width, row, row+1,
			(GtkAttachOptions) (flags), (GtkAttachOptions) (0), 0, 0);
	}
	widget = va_arg (contents, GtkWidget *);
    }

    gtk_table_resize (GTK_TABLE (table), row, max_col + 2);
    add_table_padding (table, 0, 0);
    add_table_padding (table, max_col + 1, row - 1);

    gtk_widget_show_all (table);

    return table;
}

GtkWidget *gtkutil_make_radio_group (GSList *group, GtkWidget **buttons, ...)
{
    GtkWidget *hbox;
    va_list labels;
    const char *label;
    int i = 0;

    hbox = gtk_hbox_new (TRUE, 0);

    va_start (labels, buttons);
    label = va_arg (labels, const char *);

    while (label != NULL) {
	GtkWidget *radiobutton = gtk_radio_button_new_with_label (group, label);
	group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton));
	gtk_box_pack_start (GTK_BOX (hbox), radiobutton, FALSE, FALSE, 0);

	*buttons++ = radiobutton;

	label = va_arg (labels, const char *);
    }

    gtk_widget_show (hbox);

    return hbox;
}
