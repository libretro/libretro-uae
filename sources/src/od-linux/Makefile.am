AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libosdep.a

libosdep_a_SOURCES = main.c memory.c blkdev-linux.c parser.c

noinst_HEADERS = memory_uae.h hrtimer.h
