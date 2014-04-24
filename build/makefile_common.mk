EMU = ../sources/src
LIBCOOBJ = ../sources/utils/libco
LIBUTILS =../sources/utils
LIBRETRO = ../sources/libretro
GUI = ../sources/src/gui-retro

CORE_SRCS2 :=  \
	$(EMU)/main.o $(EMU)/newcpu.o $(EMU)/memory.o $(EMU)/rommgr.o $(EMU)/custom.o \
	$(EMU)/serial.o $(EMU)/dongle.o $(EMU)/cia.o \
	$(EMU)/blitter.o $(EMU)/blittable.o $(EMU)/blitfunc.o  $(EMU)/autoconf.o $(EMU)/traps.o\
	$(EMU)/keybuf.o $(EMU)/expansion.o $(EMU)/inputrecord.o $(EMU)/keymap/keymap.o\
	$(EMU)/diskutil.o $(EMU)/zfile.o $(EMU)/zfile_archive.o $(EMU)/cfgfile.o $(EMU)/inputdevice.o \
	$(EMU)/gfxutil.o $(EMU)/gfxlib.o $(EMU)/audio.o $(EMU)/sinctable.o $(EMU)/statusline.o $(EMU)/drawing.o $(EMU)/consolehook.o \
	$(EMU)/native2amiga.o $(EMU)/disk.o $(EMU)/crc32.o $(EMU)/savestate.o $(EMU)/arcadia.o $(EMU)/cd32_fmv.o \
	$(EMU)/uaeexe.o $(EMU)/uaelib.o $(EMU)/uaeresource.o $(EMU)/uaeserial.o $(EMU)/fdi2raw.o\
	$(EMU)/hotkeys.o $(EMU)/amax.o \
	$(EMU)/ar.o $(EMU)/driveclick.o $(EMU)/enforcer.o $(EMU)/misc.o $(EMU)/uaenet.o $(EMU)/a2065.o \
	$(EMU)/missing.o $(EMU)/readcpu.o $(EMU)/hrtmon.rom.o $(EMU)/events.o $(EMU)/calc.o\
	$(EMU)/aros.rom.o $(EMU)/specialmonitors.o $(EMU)/writelog.o  $(EMU)/debug.o\
	$(EMU)/identify.o  $(EMU)/osdep/memory.o $(EMU)/osdep/parser.o $(EMU)/osdep/main.o
#$(EMU)/blkdev.o $(EMU)/blkdev_cdimage.o $(EMU)/picasso96.o $(EMU)/cdtv.o $(EMU)/akiko.o $(EMU)/a2091.o
# $(EMU)/scsi.o $(EMU)/ncr_scsi.o   $(EMU)/gayle.o

CORE_SRCS2 += $(EMU)/archivers/dms/crc_csum.o $(EMU)/archivers/dms/getbits.o $(EMU)/archivers/dms/maketbl.o \
	$(EMU)/archivers/dms/pfile.o $(EMU)/archivers/dms/tables.o \
	$(EMU)/archivers/dms/u_deep.o $(EMU)/archivers/dms/u_heavy.o \
	$(EMU)/archivers/dms/u_init.o $(EMU)/archivers/dms/u_medium.o $(EMU)/archivers/dms/u_quick.o \
	$(EMU)/archivers/dms/u_rle.o

CORE_SRCS2 += \
	$(EMU)/hardfile.o $(EMU)/hardfile_unix.o $(EMU)/scsi-none.o $(EMU)/fsusage.o \
	$(EMU)/filesys.o $(EMU)/filesys_unix.o $(EMU)/fsdb.o $(EMU)/fsdb_unix.o	$(EMU)/bsdsocket.o \
	$(EMU)/cpuemu_0.o $(EMU)/cpuemu_11.o $(EMU)/cpuemu_12.o $(EMU)/cpuemu_20.o $(EMU)/cpuemu_21.o \
	$(EMU)/cpuemu_22.o $(EMU)/cpustbl.o $(EMU)/cpudefs.o $(EMU)/cpuemu_31.o $(EMU)/cpuemu_32.o  \
	$(EMU)/cpuemu_33.o $(EMU)/cpummu.o $(EMU)/cpummu30.o $(EMU)/fpp.o 

CORE_SRCS2 += $(EMU)/machdep/support.o $(EMU)/osdep/gui.o $(EMU)/osdep/retroglue.o $(EMU)/sounddep/sound.o \
	$(EMU)/osdep/retromenu.o $(EMU)/threaddep/thread.o

DLG_SRCS = $(GUI)/dialog.o\
$(GUI)/paths.o \
$(GUI)/file.o \
$(GUI)/unzip.o \
$(GUI)/thumb.o \
$(GUI)/zip.o \
$(GUI)/str.o \
$(GUI)/dlgFloppy.o \
$(GUI)/dlgHardDisk.o \
$(GUI)/dlgFileSelect.o \
$(GUI)/dlgMemory.o \
$(GUI)/dlgJoystick.o \
$(GUI)/dlgAbout.o \
$(GUI)/dlgSound.o \
$(GUI)/dlgAlert.o \
$(GUI)/dlgMain.o \
$(GUI)/dlgMisc.o \
$(GUI)/dlgVideo.o \
$(GUI)/dlgRom.o \
$(GUI)/dlgSystem.o \
$(GUI)/sdlgui.o

LIBCO_SRCS = $(LIBCOOBJ)/libco.o 
ifeq ($(platform),android)
LIBCO_SRCS += $(LIBCOOBJ)/armeabi_asm.o
else
UNAME_M := $(shell uname -m)
ifneq ($(filter arm%,$(UNAME_M)),)
LIBCO_SRCS += $(LIBCOOBJ)/armeabi_asm.o        
endif
endif

BUILD_APP =  $(CORE_SRCS2) $(LIBCO_SRCS) $(DLG_SRCS)

HINCLUDES := -I./$(EMU) -I./$(EMU)/include -I$(LIBRETRO) -I$(LIBUTILS)

OBJECTS := $(BUILD_APP)  $(LIBRETRO)/libretro.o  $(LIBRETRO)/euae-mapper.o  $(LIBRETRO)/vkbd.o \
	 $(LIBRETRO)/graph.o  $(LIBRETRO)/diskutils.o  $(LIBRETRO)/fontmsx.o  


