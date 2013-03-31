 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Custom Gtk+ LED widget
  *
  * Copyright 2004 Martin Garton
  * Copyright 2006 Richard Drummond
  */

#ifndef __LED_H__
#define __LED_H__

#include <gdk/gdk.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LED(obj)          GTK_CHECK_CAST (obj, led_get_type (), Led)
#define LED_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, led_get_type (), LedClass)
#define IS_LED(obj)       GTK_CHECK_TYPE (obj, led_get_type ())

typedef struct _Led       Led;
typedef struct _LedClass  LedClass;

static const GdkColor LED_OFF = {0, 0x1111, 0x1111, 0x1111};

struct _Led
{
    GtkWidget  widget;

    GdkGC     *gc;
    GdkColor   color;
};

struct _LedClass
{
  GtkWidgetClass parent_class;
};

guint			led_get_type	(void);
GtkWidget*		led_new		(void);
void			led_set_off	(Led *theled);
void			led_set_color	(Led *theled, GdkColor col);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LED_H__ */
