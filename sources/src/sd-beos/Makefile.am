AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CFLAGS@ -Wno-multichar

noinst_LIBRARIES = libsnddep.a

libsnddep_a_SOURCES = sound.cpp

noinst_HEADERS = sound.h
