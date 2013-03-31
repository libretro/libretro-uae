/* floppyfileentry.h */

#ifndef __FLOPPYFILEENTRY_H__
#define __FLOPPYFILEENTRY_H__

#include <gdk/gdk.h>
#include <gtk/gtkframe.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FLOPPYFILEENTRY(obj)          GTK_CHECK_CAST (obj, floppyfileentry_get_type (), FloppyFileEntry)
#define FLOPPYFILEENTRY_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, floppyfileentry_get_type (), FloppyFileEntryClass)
#define IS_FLOPPYFILEENTRY(obj)       GTK_CHECK_TYPE (obj, floppyfileentry_get_type ())

typedef struct _FloppyFileEntry       FloppyFileEntry;
typedef struct _FloppyFileEntryClass  FloppyFileEntryClass;

struct _FloppyFileEntry
{
    GtkFrame   frame;
    GtkWidget *led;
    GtkWidget *insert_button;
    GtkWidget *eject_button;
    GtkWidget *path_widget;

    gchar     *filename;
    gchar     *drivename;

    GtkWidget *filesel;
};

struct _FloppyFileEntryClass
{
  GtkFrameClass parent_class;

  void (* floppyfileentry) (FloppyFileEntry *floppyfileentry);
};

guint			floppyfileentry_get_type	(void);
GtkWidget*		floppyfileentry_new		(void);
void			floppyfileentry_set_currentdir	(FloppyFileEntry *ffentry, const gchar *filename);
void			floppyfileentry_set_filename	(FloppyFileEntry *ffentry, const gchar *filename);
void			floppyfileentry_set_label	(FloppyFileEntry *ffentry, const gchar *filename);
void			floppyfileentry_set_drivename	(FloppyFileEntry *ffentry, const gchar *filename);
#if GTK_MAJOR_VERSION > 2
G_CONST_RETURN
#endif
gchar		       *floppyfileentry_get_filename	(FloppyFileEntry *ffentry);
void			floppyfileentry_set_led		(FloppyFileEntry *ffentry, gboolean state);
void			floppyfileentry_do_dialog	(FloppyFileEntry *ffentry);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FLOPPYFILEENTRY_H__ */
