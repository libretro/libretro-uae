/*
 * chipsetspeedpanel.c
 *
 * Part of the Gtk+ UI for E-UAE
 * Copyright 2003-2004 Richard Drummond
 */

#ifndef __CHIPSETSPEEDPANEL_H__
#define __CHIPSETSPEEDPANEL_H__

#include <gdk/gdk.h>
#include <gtk/gtkframe.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CHIPSETSPEEDPANEL(obj)          GTK_CHECK_CAST (obj, chipsetspeedpanel_get_type (), ChipsetSpeedPanel)
#define CHIPSETSPEEDPANEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, chipsetspeedpanel_get_type (), ChipsetSpeedPanelClass)
#define IS_CHIPSETSPEEDPANEL(obj)       GTK_CHECK_TYPE (obj, chipsetspeedpanel_get_type ())

typedef struct _ChipsetSpeedPanel       ChipsetSpeedPanel;
typedef struct _ChipsetSpeedPanelClass  ChipsetSpeedPanelClass;

struct _ChipsetSpeedPanel
{
    GtkFrame    frame;
    GtkWidget  *framerate_widget;
    GtkWidget  *collisions_widget;
    GtkWidget  *immediate_blits_widget;

    guint       framerate;
    guint       collision_level;
    guint       immediate_blits;
};

struct _ChipsetSpeedPanelClass
{
  GtkFrameClass parent_class;

  void (* chipsetspeedpanel) (ChipsetSpeedPanel *chipsetspeedpanel );
};

guint		chipsetspeedpanel_get_type	  (void);
GtkWidget*	chipsetspeedpanel_new		  (void);

void chipsetspeedpanel_set_framerate		  (ChipsetSpeedPanel *panel, guint framerate);
void chipsetspeedpanel_set_collision_level	  (ChipsetSpeedPanel *panel, guint collision_level);
void chipsetspeedpanel_set_immediate_blits	  (ChipsetSpeedPanel *panel, guint immediate_blits);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHIPSETSPEEDPANEL_H__ */
