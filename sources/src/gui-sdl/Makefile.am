AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@ @SDL_CFLAGS@
if BUILD_MACOSX_BUNDLE
AM_CFLAGS   += -I/Library/Frameworks/SDL_image.framework/Headers -I/Library/Frameworks/SDL_ttf.framework/Headers
endif
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = libguidep.a

libguidep_a_SOURCES = inputmode.c menu.c menu_load.c menu_prefs.c \
		      vkbd.cpp volumecontrol.c

noinst_HEADERS = inputmode.h menu.h vkbd.h volumecontrol.h
