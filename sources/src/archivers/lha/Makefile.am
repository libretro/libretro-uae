AM_CPPFLAGS  = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src
AM_CFLAGS    = @UAE_CFLAGS@
AM_CXXFLAGS  = @UAE_CXXFLAGS@

noinst_LIBRARIES = liblha.a

liblha_a_SOURCES = crcio.c dhuf.c header.c huf.c larc.c \
		   lhamaketbl.c lharc.c shuf.c slide.c uae_lha.c \
		   util.c

noinst_HEADERS   = lha.h lha_macro.h
