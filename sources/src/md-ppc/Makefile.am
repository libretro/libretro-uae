AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libmachdep.a

libmachdep_a_SOURCES = support.c

noinst_HEADERS = m68k.h maccess.h rpt.h
