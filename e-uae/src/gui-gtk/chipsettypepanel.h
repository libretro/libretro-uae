/* chipsettypepanel.h */

#ifndef __CHIPSETTYPEPANEL_H__
#define __CHIPSETTYPEPANEL_H__

#include <gdk/gdk.h>
#include <gtk/gtkframe.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CHIPSETTYPEPANEL(obj)          GTK_CHECK_CAST (obj, chipsettypepanel_get_type (), ChipsetTypePanel)
#define CHIPSETTYPEPANEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, chipsettypepanel_get_type (), ChipsetTypePanelClass)
#define IS_CHIPSETTYPEPANEL(obj)       GTK_CHECK_TYPE (obj, chipsettypepanel_get_type ())

typedef struct _ChipsetTypePanel       ChipsetTypePanel;
typedef struct _ChipsetTypePanelClass  ChipsetTypePanelClass;

struct _ChipsetTypePanel
{
    GtkFrame   frame;
    GtkWidget  *chipsettype_widget;
    GtkWidget  *frequency_widget[2];

    guint      chipset_mask;
    guint      ntscmode;
};

struct _ChipsetTypePanelClass
{
  GtkFrameClass parent_class;

  void (* chipsettypepanel) (ChipsetTypePanel *chipsettypepanel );
};

guint		chipsettypepanel_get_type	  (void);
GtkWidget*	chipsettypepanel_new		  (void);
void            chipsettypepanel_set_chipset_mask (ChipsetTypePanel *panel, guint chipset_mask);
void            chipsettypepanel_set_ntscmode     (ChipsetTypePanel *panel, guint ntscmode);   

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHIPSETTYPEPANEL_H__ */
