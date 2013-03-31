INCLUDES =  -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src -I$(top_srcdir)

noinst_LIBRARIES = libkeymap.a

libkeymap_a_SOURCES = keymap.c x11pc_rawkeys.c quartz_rawkeys.c beos_rawkeys.c \
		      amiga_rawkeys.c

noinst_HEADERS = keymap.h x11pc_rawkeys.h quartz_rawkeys.h beos_rawkeys.h \
	         amiga_rawkeys.h keymap_common.h keymap_all.h hotkeys_common.h
