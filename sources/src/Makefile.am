AM_CFLAGS = @UAE_CFLAGS@
AM_CPPFLAGS = @UAE_CPPFLAGS@
AM_CPPFLAGS += -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src -I$(top_srcdir)
AM_CXXFLAGS  = @UAE_CXXFLAGS@

LIBS = -L/usr/lib -lstdc++
if !ANDROID
LIBS += -lpthread
endif
if IS_QT
QT_INCPATH = $(shell qmake -query QT_INSTALL_HEADERS)
QT_INSPATH = $(shell qmake -query QT_INSTALL_DATA)
QT_INSLIBS = $(shell qmake -query QT_INSTALL_LIBS)
AM_CPPFLAGS += -I$(QT_INCPATH) -I$(QT_INCPATH)/QtCore -I$(QT_INCPATH)/QtGui
AM_CPPFLAGS += -I$(QT_INSPATH)/mkspecs/default
if BUILD_MACOSX_BUNDLE
AM_CPPFLAGS += -I$(QT_INSPATH)/mkspecs/macx-xcode -I$(QT_INSPATH)/mkspecs/macx-g++
AM_CPPFLAGS += -I/System/Library/Frameworks/CarbonCore.framework/Headers
AM_CPPFLAGS += -F$(QT_INSLIBS)
AM_CPPFLAGS += -I$(QT_INSLIBS)/QtCore.framework/Versions/4/Headers
AM_CPPFLAGS += -I$(QT_INSLIBS)/QtGui.framework/Versions/4/Headers
LIBS += -F$(QT_INSLIBS) -L$(QT_INSLIBS) -framework QtGui -framework QtCore 
else
LIBS += -L$(QT_INSLIBS) -lQtGui -lQtCore
endif
DEFINES = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
endif

SUBDIRS = \
	tools @MACHDEP@ @THREADDEP@ @GFX_DEP@ @SND_DEP@ @JOY_DEP@ @GUI_DEP@ \
	@OSDEP@ keymap archivers/dms archivers/zip caps

DIST_SUBDIRS = \
	md-generic md-68k md-i386-gcc md-ppc md-ppc-gcc md-amd64-gcc \
	gfx-amigaos gfx-beos gfx-x11 gfx-sdl gfx-curses gfx-svga gfx-cocoa \
	sd-none sd-alsa sd-amigaos sd-beos sd-sdl sd-solaris sd-uss \
	jd-none jd-amigainput jd-amigaos jd-beos jd-linuxold jd-sdl \
	gui-none gui-beos gui-cocoa gui-gtk gui-muirexx gui-sdl \
	od-generic od-amiga od-beos od-linux od-macosx \
	td-none td-amigaos td-beos td-posix td-sdl \
	keymap caps archivers/dms archivers/zip

BUILT_SOURCES = \
	blit.h blitfunc.h blitfunc.c blittable.c \
	linetoscr.c \
	cpudefs.c \
	cpuemu_0.c cpuemu_11.c cpuemu_12.c cpuemu_20.c cpuemu_21.c cpuemu_22.c cpuemu_31.c cpuemu_32.c cpuemu_33.c \
	cpustbl.c cputbl.h \
	compemu.c \
	compstbl.c comptbl.h

CLEANFILES = \
	blit.h blitfunc.h blitfunc.c blittable.c \
	cpudefs.c \
	cpuemu_0.c cpuemu_11.c cpuemu_12.c cpuemu_20.c cpuemu_21.c cpuemu_22.c cpuemu_31.c cpuemu_32.c cpuemu_32.c \
	cpustbl.c cputbl.h \
	compemu.c \
	compstbl.c comptbl.h \
	linetoscr.c

DISTCLEANFILES = \
	machdep osdep threaddep gfxdep sounddep joydep guidep \
	target.h md-fpp.h

bin_PROGRAMS  = \
	uae readdisk make_hdf

if BUILD_MACOSX_BUNDLE
bundle = @PACKAGE_NAME@.app
bundle_contents = $(bundle)/Contents
endif

if TARGET_BIGENDIAN
genlinetoscr_args = -b
endif

uae_extraLDFLAGS=-fwhole-program

uae$(EXEEXT):	$(uae_OBJECTS) $(uae_DEPENDENCIES)
		@rm -f uae$(EXEEXT)
		$(LINK) $(uae_LDFLAGS) $(uae_extraLDFLAGS) $(uae_OBJECTS) $(uae_LDADD) $(LIBS)

if TARGET_BEOS
		xres -o $@ @UAE_RSRCFILE@
		mimeset -f $@
endif

if BUILD_MACOSX_BUNDLE
all-local: $(bundle)
$(bundle): uae$(EXEEXT) od-macosx/Info.plist od-macosx/puae.icns od-macosx/Credits.rtf
		rm -rf $(bundle)
		mkdir -p $(bundle_contents)/MacOS
		mkdir -p $(bundle_contents)/MacOS/uae_data
		mkdir -p $(bundle_contents)/Resources
		mkdir -p $(bundle_contents)/Frameworks
		$(INSTALL_DATA) od-macosx/Info.plist $(bundle_contents)
		$(INSTALL_PROGRAM) uae$(EXEEXT) $(bundle_contents)/MacOS/
		cp -R /Library/Frameworks/SDL.framework $(bundle_contents)/Frameworks
		cp -R $(top_srcdir)/uae_data $(bundle_contents)/MacOS
if SDL_UI
		mkdir -p $(bundle_contents)/MacOS/guidep
		cp -R /Library/Frameworks/SDL_image.framework $(bundle_contents)/Frameworks
		cp -R /Library/Frameworks/SDL_ttf.framework $(bundle_contents)/Frameworks
		cp -R $(srcdir)/gui-sdl/fonts $(bundle_contents)/MacOS/guidep
		cp -R $(srcdir)/gui-sdl/images $(bundle_contents)/MacOS/guidep
endif
		$(INSTALL_DATA) od-macosx/puae.icns $(bundle_contents)/Resources/
		$(INSTALL_DATA) od-macosx/Credits.rtf $(bundle_contents)/Resources/
endif

noinst_LIBRARIES = \
	libcpuemu.a

dist_noinst_DATA = \
	table68k inputevents.def filesys.asm

noinst_HEADERS = \
	include/akiko.h		include/ar.h		include/amax.h \
	include/audio.h		include/autoconf.h	\
	include/blitter.h	include/blkdev.h	\
	include/bsdsocket.h 	include/caps.h		\
	include/catweasel.h     include/cdrom.h		\
	include/cia.h		include/cfgfile.h       \
	include/commpipe.h	include/compemu.h	\
	include/cpu_prefetch.h  include/custom.h	\
	include/custom_private.h                        \
	include/crc32.h		include/consolehook.h	\
	include/arcadia.h	include/cdtv.h		\
	include/debug.h		include/disk.h 		\
	include/dongle.h	include/a2065.h		\
	include/gayle.h		include/a2091.h		include/ncr_scsi.h	\
	include/drawing.h 	include/driveclick.h	\
	include/enforcer.h	\
	include/events.h 	\
	include/execlib.h 	\
	include/fdi2raw.h	include/filesys.h	\
	include/fpp-ieee.h	include/fpp-unknown.h	\
	include/fsdb.h		include/fsusage.h	\
	include/genblitter.h	include/gensound.h	\
	include/gfxfilter.h	include/gui.h		\
	include/hotkeys.h				\
	include/hrtimer.h	include/identify.h	\
	include/inputdevice.h	include/joystick.h	\
	include/keyboard.h	include/keybuf.h	\
	include/memory_uae.h	\
	include/native2amiga.h	include/newcpu.h	\
	include/noflags.h	include/options.h	\
	include/osemu.h		include/picasso96.h	\
	include/readcpu.h	include/savestate.h	\
	include/scsidev.h	include/serial.h	\
	include/sana2.h		\
	include/sleep.h		include/sysdeps.h	\
	include/traps.h                                 \
	include/tui.h		include/uae.h		\
	include/uaeexe.h	include/uaenet.h	\
	include/uae_endian.h \
	include/uae_malloc.h \
	include/uae_string.h \
	include/uae_types.h \
	include/writelog.h \
	include/xwin.h		include/zfile.h		include/diskutil.h \
	targets/t-amiga.h	targets/t-beos.h        \
	targets/t-unix.h\
	include/gcc_warnings.h

dist_noinst_SCRIPTS = \
	filesys.sh install_libscg

EXTRA_DIST = \
	tools/configure.in tools/configure tools/sysconfig.h.in \
	tools/target.h tools/Makefile.in \
	test/test_optflag.c test/Makefile.in test/Makefile.am

uae_SOURCES = \
	main.c newcpu.c memory.c rommgr.c custom.c serial.c dongle.c cia.c \
	blitter.c autoconf.c traps.c keybuf.c expansion.c inputrecord.c \
	diskutil.c zfile.c zfile_archive.c cfgfile.c picasso96.c inputdevice.c \
	gfxutil.c audio.c sinctable.c statusline.c drawing.c consolehook.c \
	native2amiga.c disk.c crc32.c savestate.c arcadia.c cdtv.c cd32_fmv.c \
	uaeexe.c uaelib.c uaeresource.c uaeserial.c fdi2raw.c hotkeys.c amax.c \
	ar.c driveclick.c enforcer.c misc.c uaenet.c a2065.c gayle.c blkdev.c blkdev_cdimage.c scsi.c ncr_scsi.c \
	missing.c readcpu.c hrtmon.rom.c events.c calc.c aros.rom.c specialmonitors.c

EXTRA_uae_SOURCES = \
	bsdsocket.c bsdsocket-posix-new.c build68k.c catweasel.c cdrom.c \
	fpp.c compemu_fpp.c compemu_support.c \
	debug.c identify.c filesys_bootrom.c fsdb.c fsdb_unix.c fsusage.c genblitter.c \
	gencpu.c gengenblitter.c gencomp.c genlinetoscr.c hardfile.c \
	hardfile_unix.c scsi-none.c scsiemul.c svgancui.c tui.c amax.c \
	akiko.c cd32_fmv.c ar.c blkdev-libscg.c \
	blitops.c writelog.c

uae_DEPENDENCIES = \
	@MACHDEP@/libmachdep.a		\
	@JOY_DEP@/libjoydep.a		\
	@SND_DEP@/libsnddep.a		\
	@GFX_DEP@/libgfxdep.a		\
	@GUI_DEP@/libguidep.a		\
	keymap/libkeymap.a		\
	archivers/dms/libdms.a		\
	archivers/zip/libzip.a		\
	caps/libcaps.a			\
	blitfunc.o blittable.o		\
	cpustbl.o cpudefs.o		\
	libcpuemu.a			\
	@EXTRAOBJS@			\
	@FILESYSOBJS@ 			\
	@BSDSOCKOBJS@			\
	@SCSIOBJS@			\
	@CDOBJS@			\
	@DEBUGOBJS@			\
	@ASMOBJS@			\
	@THREADDEP@/libthreaddep.a	\
	@OSDEP@/libosdep.a

uae_LDADD = \
	@MACHDEP@/libmachdep.a		\
	@JOY_DEP@/libjoydep.a		\
	@SND_DEP@/libsnddep.a		\
	@GFX_DEP@/libgfxdep.a		\
	@GUI_DEP@/libguidep.a		\
	keymap/libkeymap.a		\
	archivers/dms/libdms.a		\
	archivers/zip/libzip.a		\
	caps/libcaps.a			\
	blitfunc.o blittable.o		\
	cpustbl.o cpudefs.o		\
	libcpuemu.a			\
	@EXTRAOBJS@			\
	@FILESYSOBJS@			\
	@BSDSOCKOBJS@			\
	@SCSIOBJS@			\
	@CDOBJS@			\
	@DEBUGOBJS@			\
	@ASMOBJS@			\
	@THREADDEP@/libthreaddep.a	\
	@OSDEP@/libosdep.a

uae_LDADD += @UAE_LIBS@ @MATHLIB@

readdisk_SOURCES = \
	readdisk.c missing.c

make_hdf_SOURCES = \
	make_hdf.c

libcpuemu_a_SOURCES =
libcpuemu_a_LIBADD =		@CPUOBJS@ @JITOBJS@
libcpuemu_a_DEPENDENCIES =	@CPUOBJS@ @JITOBJS@

cpuemu_0.c:	tools/gencpu
		./tools/gencpu @GENCPUOPTS@

cpustbl.c:	cpuemu_0.c

cputbl.h:	cpuemu_0.c

cpuemu_11.c:	cpuemu_0.c
cpuemu_12.c:	cpuemu_0.c
cpuemu_20.c:	cpuemu_0.c
cpuemu_21.c:	cpuemu_0.c
cpuemu_22.c:	cpuemu_0.c
cpuemu_31.c:	cpuemu_0.c
cpuemu_32.c:	cpuemu_0.c
cpuemu_33.c:	cpuemu_0.c

compemu.c:	tools/gencomp
		./tools/gencomp

compstbl.c:	compemu.c

comptbl.h:	compemu.c

cpuemu_nf_%.o: cpuemu_%.c
	$(COMPILE) -DNOFLAGS $< -c -o $@

cpuemu_%.o: cpuemu_%.c
	$(COMPILE)  $< -c -o $@

cpustbl_nf.o: cpustbl.c
	$(COMPILE) -DNOFLAGS cpustbl.c -c -o $@


blit.h:		tools/genblitter
		./tools/genblitter i >blit.h

blitfunc.c: 	tools/genblitter blitfunc.h
		./tools/genblitter f >blitfunc.c

blitfunc.h: 	tools/genblitter
		./tools/genblitter h >blitfunc.h

blittable.c: 	tools/genblitter blitfunc.h
		./tools/genblitter t >blittable.c

linetoscr.c:    tools/genlinetoscr
		./tools/genlinetoscr $(genlinetoscr_args) >linetoscr.c

tools/genblitter:
	$(MAKE) -C tools genblitter
tools/build68k:
	$(MAKE) -C tools build68k
tools/gencpu:
	$(MAKE) -C tools gencpu
tools/gencomp:
	$(MAKE) -C tools gencomp
tools/genlinetoscr:
	$(MAKE) -C tools genlinetoscr

cpudefs.c:	tools/build68k @top_srcdir@/src/table68k
		sleep 1s
		./tools/build68k <@top_srcdir@/src/table68k >cpudefs.c

machdep/libmachdep.a:
	$(MAKE) -C @MACHDEP@ libmachdep.a

gfxdep/libgfxdep.a:
	$(MAKE) -C @GFX_DEP@ libgfxdep.a

sounddep/libsnddep.a:
	$(MAKE) -C @SND_DEP@ libsnddep.a

joydep/libjoydep.a:
	$(MAKE)	-C @JOY_DEP@ libjoydep.a

guidep/libguidep.a:
	$(MAKE) -C @GUI_DEP@ libguidep.a

osdep/libosdep.a:
	$(MAKE) -C @OSDEP@ libosdep.a

osdep/libthreaddep.a:
	$(MAKE) -C @THREADDEP@ libthreaddep.a

keymap/libkeymap.a:
	$(MAKE) -C keymap libkeymap.a

clean-local:
	$(MAKE) -C tools clean

distclean-local:
	$(MAKE) -C tools distclean
