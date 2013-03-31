 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Custom Gtk+ LED widget
  *
  * Copyright 2004 Martin Garton
  * Copyright 2006 Richard Drummond
  */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>

#include "led.h"

#define LED_W 20
#define LED_H 10

static void led_init (Led *theled);
static void led_class_init (LedClass *class);
static gint led_expose (GtkWidget *w, GdkEventExpose *event);
static void led_destroy (GtkObject *object);
static void led_realize (GtkWidget *widget);
static void led_unrealize (GtkWidget *widget);
static void led_size_request (GtkWidget *widget, GtkRequisition *requisition);
static void led_size_allocate (GtkWidget *widget, GtkAllocation *allocation);


guint led_get_type ()
{
    static guint led_type = 0;

    if (!led_type) {
	static const GtkTypeInfo led_info = {
	    (char *) "Led",
	    sizeof (Led),
	    sizeof (LedClass),
	    (GtkClassInitFunc) led_class_init,
	    (GtkObjectInitFunc) led_init,
	    NULL,
	    NULL,
	    (GtkClassInitFunc) NULL
	};
	led_type = gtk_type_unique (gtk_misc_get_type (), &led_info);
    }
    return led_type;
}

static GtkObjectClass *parent_class;

static void led_class_init (LedClass *class)
{
    GtkObjectClass *object_class = (GtkObjectClass *) class;
    GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
    parent_class = gtk_type_class (gtk_widget_get_type ());

    object_class->destroy = led_destroy;
    widget_class->expose_event = led_expose;
    widget_class->realize = led_realize;
    widget_class->unrealize = led_unrealize;
    widget_class->size_request = led_size_request;
    widget_class->size_allocate = led_size_allocate;
}

static void led_init (Led *theled)
{
    theled->color = LED_OFF;
}

GtkWidget *led_new (void)
{
    return gtk_type_new (led_get_type ());
}

static gint led_expose (GtkWidget *w, GdkEventExpose *event)
{
    if (w && GTK_WIDGET_DRAWABLE (w)) {
	Led *theled = LED (w);
	gdk_draw_rectangle (w->window, theled->gc, TRUE, 0, 0,
			    w->allocation.width, w->allocation.height);
    }
    return 0;
}

static void led_realize (GtkWidget *widget)
{
    Led *theled;
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (IS_LED (widget));

    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    theled = LED (widget);

    attributes.x           = widget->allocation.x;
    attributes.y           = widget->allocation.y;
    attributes.width       = widget->allocation.width;
    attributes.height      = widget->allocation.height;
    attributes.wclass      = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask  = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
    attributes.visual      = gtk_widget_get_visual (widget);
    attributes.colormap    = gtk_widget_get_colormap (widget);

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);

    gdk_window_set_user_data (widget->window, widget);

    theled->gc = gdk_gc_new (widget->window);
    gdk_gc_set_rgb_fg_color (theled->gc, &theled->color);

    led_expose (widget, NULL);
}

static void led_unrealize (GtkWidget *widget)
{
    Led *theled = LED (widget);

    g_object_unref (theled->gc);
    theled->gc = NULL;

    GTK_WIDGET_CLASS (parent_class)->unrealize (widget);
}

static void led_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
    requisition->width  = LED_W;
    requisition->height = LED_H;
}

static void led_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    Led *theled = LED (widget);

    g_return_if_fail (widget != NULL);
    g_return_if_fail (IS_LED (widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget)) {
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
     }
}

static void led_destroy (GtkObject *o)
{
    g_return_if_fail (o != NULL);
    g_return_if_fail (IS_LED (o));

    // TODO: ?? free any stuff here.

    GTK_OBJECT_CLASS (parent_class)->destroy (o);
}

void led_set_color (Led *l, GdkColor col)
{
    l->color = col;

    if (GTK_WIDGET_REALIZED (l)) {
	gdk_gc_set_rgb_fg_color (l->gc, &l->color);
	led_expose (GTK_WIDGET (l), NULL);
    }
}
