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

#define FPUTYPEPANEL(obj)          GTK_CHECK_CAST (obj, fputypepanel_get_type (), FpuTypePanel)
#define FPUTYPEPANEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, fputypepanel_get_type (), FpuTypePanelClass)
#define IS_FPUTYPEPANEL(obj)       GTK_CHECK_TYPE (obj, fputype_panel_get_type ())

typedef struct _CpuTypePanel       CpuTypePanel;
typedef struct _CpuTypePanelClass  CpuTypePanelClass;

typedef struct _FpuTypePanel       FpuTypePanel;
typedef struct _FpuTypePanelClass  FpuTypePanelClass;

struct _CpuTypePanel
{
	GtkFrame   frame;
	GtkWidget	*cputype_widget;
	GtkWidget	*addr24bit_widget;
	GtkWidget	*cpu_morecompat_widget;
	GtkWidget	*mmu40_widget;
	GtkWidget	*accuracy_widget;
	guint		cputype;
	guint		addr24bit;
	guint		cpumorecompat;
	guint		mmu40;
	guint		cycleexact;
	guint		fpuenabled;
};

struct _FpuTypePanel
{
    GtkFrame	frame;
    GtkWidget	*fputype_widget;
    GtkWidget	*fpu_morecompat_widget;
	guint		fputype;
	guint		fpumorecompat;
};

struct _CpuTypePanelClass
{
  GtkFrameClass parent_class;

  void (* cputypepanel) (CpuTypePanel *cputypepanel );
};

struct _FpuTypePanelClass
{
  GtkFrameClass parent_class;

  void (* fputypepanel) (FpuTypePanel *fputypepanel );
};

GtkType		cputypepanel_get_type	(void);
GtkWidget*	cputypepanel_new		(void);
void		cputypepanel_set_cpulevel	(CpuTypePanel *ctpanel, guint cpulevel);
void		cputypepanel_set_addr24bit	(CpuTypePanel *ctpanel, guint addr24bit);
guint		cputypepanel_get_cpulevel	(CpuTypePanel *ctpanel);
guint		cputypepanel_get_addr24bit	(CpuTypePanel *ctpanel);
void		cputypepanel_set_compatible	(CpuTypePanel *ctpanel, gboolean compatible);
void		cputypepanel_set_cycleexact	(CpuTypePanel *ctpanel, gboolean cycleexact);

guint		fputypepanel_get_type	(void);
GtkWidget*	fputypepanel_new		(void);
void		fputypepanel_set_fpulevel	(FpuTypePanel *ftpanel, guint fpulevel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CPUTYPEPANEL_H__ */
