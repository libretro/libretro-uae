AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libgfxdep.a

libgfxdep_a_SOURCES = sdlgfx.c sdlkeys.c sdlkeys_dik.c rawkeys.c

noinst_HEADERS = gfx.h sdlgfx.h sdlkeys_dik.h
