/* cputypepanel.h */

#ifndef __CPUTYPEPANEL_H__
#define __CPUTYPEPANEL_H__

#include <gdk/gdk.h>
#include <gtk/gtkframe.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPUTYPEPANEL(obj)          GTK_CHECK_CAST (obj, cputypepanel_get_type (), CpuTypePanel)
#define CPUTYPEPANEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, cputypepanel_get_type (), CpuTypePanelClass)
#define IS_CPUTYPEPANEL(obj)       GTK_CHECK_TYPE (obj, cputype_panel_get_type ())

typedef struct _CpuTypePanel       CpuTypePanel;
typedef struct _CpuTypePanelClass  CpuTypePanelClass;

struct _CpuTypePanel
{
    GtkFrame   frame;
    GtkWidget *cputype_widget;
    GtkWidget *addr24bit_widget;
    GtkWidget *fpuenabled_widget;
    GtkWidget *accuracy_widget;
    guint      cputype;
    guint      addr24bit;
    guint      fpuenabled;
    guint      compatible;
    guint      cycleexact;
};

struct _CpuTypePanelClass
{
  GtkFrameClass parent_class;

  void (* cputypepanel) (CpuTypePanel *cputypepanel );
};

guint		cputypepanel_get_type		(void);
GtkWidget*	cputypepanel_new		(void);
void		cputypepanel_set_cpulevel	(CpuTypePanel *ctpanel, guint cpulevel);
void		cputypepanel_set_addr24bit	(CpuTypePanel *ctpanel, guint addr24bit);
guint		cputypepanel_get_cpulevel	(CpuTypePanel *ctpanel);
guint		cputypepanel_get_addr24bit	(CpuTypePanel *ctpanel);
void		cputypepanel_set_compatible	(CpuTypePanel *ctpanel, gboolean compatible);
void		cputypepanel_set_cycleexact	(CpuTypePanel *ctpanel, gboolean cycleexact);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CPUTYPEPANEL_H__ */
