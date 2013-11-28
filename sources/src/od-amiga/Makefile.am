AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libosdep.a

libosdep_a_SOURCES  = main.c memory.c support.c ami-disk.c blkdev-amiga.c parser.c

noinst_HEADERS = amiga-kludges.h memory_uae.h hrtimer.h

dist_noinst_SCRIPTS = scc
