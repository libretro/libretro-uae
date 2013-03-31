INCLUDES =  -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src -I$(top_srcdir)

AM_CFLAGS = @GTK_CFLAGS@

noinst_LIBRARIES = libguidep.a

libguidep_a_SOURCES = gtkui.c util.c chooserwidget.c cputypepanel.c \
		      cpuspeedpanel.c floppyfileentry.c led.c \
		      chipsettypepanel.c chipsetspeedpanel.c

noinst_HEADERS = util.h chooserwidget.h cputypepanel.h cpuspeedpanel.h \
		 floppyfileentry.h led.h chipsettypepanel.h chipsetspeedpanel.h

