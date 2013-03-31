/*
 * chooserwidget.c
 *
 * Copyright 2003-2004 Richard Drummond
 */

#ifndef __CHOOSERWIDGET_H__
#define __CHOOSERWIDGET_H__

#include <gdk/gdk.h>
#include <gtk/gtkcombo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CHOOSERWIDGET(obj)          GTK_CHECK_CAST (obj, chooserwidget_get_type (), ChooserWidget)
#define CHOOSERWIDGET_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, chooserwidget_get_type (), ChooserWidgetClass)
#define IS_CHOOSERWIDGET(obj)       GTK_CHECK_TYPE (obj, chooserwidget_get_type ())


typedef struct _ChooserWidget      ChooserWidget;
typedef struct _ChooserWidgetClass ChooserWidgetClass;

struct _ChooserWidget
{
    GtkCombo combo;
    guint    choice;
//    gboolean popup_visible;
};

struct _ChooserWidgetClass
{
    GtkComboClass parent_class;
    void (* chooserwidget) (ChooserWidget *chooserwidget);
};

guint		chooserwidget_get_type		(void);
GtkWidget*	chooserwidget_new		(void);
void		chooserwidget_clear		(ChooserWidget *chooser);
void		chooserwidget_set_choice	(ChooserWidget *chooser, guint choice_num);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHOOSERWIDGET_H__ */
