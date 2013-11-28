AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libguidep.a

libguidep_a_SOURCES = ami-rexx.c ami-gui.c

dist_bin_SCRIPTS    = UAEWB UAEWB.aux UAEWB.rexx UAEGUI.rexx
