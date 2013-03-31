LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_DIR := ../../src

LOCAL_MODULE    := retro-euae

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -DANDROID_ARM
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS +=  -DANDROID_X86
endif

ifeq ($(TARGET_ARCH),mips)
LOCAL_CFLAGS += -DANDROID_MIPS -D__mips__ -D__MIPSEL__
endif


EMU = ../e-uae/src

CORE_SRCS := $(EMU)/main.c $(EMU)/newcpu.c $(EMU)/fpp.c $(EMU)/memory.c $(EMU)/custom.c $(EMU)/serial.c $(EMU)/cia.c \
	$(EMU)/blitter.c $(EMU)/blittable.c $(EMU)/blitfunc.c $(EMU)/ersatz.c $(EMU)/keybuf.c $(EMU)/expansion.c \
	$(EMU)/cfgfile.c $(EMU)/inputdevice.c $(EMU)/gfxutil.c $(EMU)/audio.c $(EMU)/drawing.c \
	$(EMU)/identify.c $(EMU)/disk.c $(EMU)/savestate.c $(EMU)/uaelib.c $(EMU)/fdi2raw.c \
	$(EMU)/hotkeys.c $(EMU)/enforcer.c  $(EMU)/missing.c \
	$(EMU)/readcpu.c $(EMU)/writelog.c $(EMU)/unzip.c $(EMU)/zfile.c $(EMU)/sinctable.c

CORE_SRCS += $(EMU)/hardfile.c $(EMU)/hardfile_unix.c $(EMU)/scsi-none.c $(EMU)/fsusage.c  $(EMU)/crc32.c \
	$(EMU)/events.c $(EMU)/misc.c 

#PPU_SRCS += blkdev.c scsiemul.c

# cpu code
CORE_SRCS +=   $(EMU)/cpustbl.c $(EMU)/cpudefs.c  $(EMU)/cpuemu_0.c 
CORE_SRCS += $(EMU)/cpuemu_5.c $(EMU)/cpuemu_6.c 

CORE_SRCS += $(EMU)/ar.c $(EMU)/autoconf.c $(EMU)/traps.c $(EMU)/filesys.c $(EMU)/filesys_unix.c $(EMU)/fsdb.c $(EMU)/fsdb_unix.c	\
		$(EMU)/native2amiga.c $(EMU)/uaeexe.c $(EMU)/bsdsocket.c $(EMU)/driveclick.c
# zlib
#PPU_SRCS += zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c \
#	zlib/uncompr.c  zlib/trees.c zlib/inflate.c zlib/infback.c \
#	zlib/inftrees.c zlib/inffast.c zlib/zutil.c

#dms
CORE_SRCS += $(EMU)/dms/crc_csum.c $(EMU)/dms/getbits.c $(EMU)/dms/maketbl.c $(EMU)/dms/pfile.c $(EMU)/dms/tables.c \
	$(EMU)/dms/u_deep.c $(EMU)/dms/u_heavy.c $(EMU)/dms/u_init.c $(EMU)/dms/u_medium.c $(EMU)/dms/u_quick.c \
	$(EMU)/dms/u_rle.c
# libretro port code
CORE_SRCS += $(EMU)/machdep/support.c $(EMU)/osdep/gui.c $(EMU)/osdep/retroglue.c $(EMU)/sounddep/sound.c \
	$(EMU)/osdep/retromenu.c


BUILD_APP =  $(CORE_SRCS) 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(EMU)   $(LOCAL_PATH)/../libretro $(LOCAL_PATH)/$(EMU)/machdep $(LOCAL_PATH)/$(EMU)/include 

OBJECTS :=   ../libretro/libretro-euae.c ../libretro/euae-mapper.c ../libretro/vkbd.c \
	../libretro/graph.c ../libretro/diskutils.c ../libretro/fontmsx.c  $(BUILD_APP)

DEFINES += -DFPUEMU -DCPUEMU_0 -DCPUEMU_5 -DCPUEMU_6 

DEFINES +=  -DAGA -DFDI2RAW -DFILESYS -DAUTOCONFIG -DSAVESTATE -DENFORCER -DACTION_REPLAY -DSUPPORT_THREADS=0 

LOCAL_SRC_FILES    += $(OBJECTS)

LOCAL_CFLAGS +=   $(DEFINES) -DRETRO=1 -DAND -D_ANDROID_ \
		-std=gnu99  -O3 -finline-functions -funroll-loops  -fsigned-char  \
		 -Wno-strict-prototypes -ffast-math -fomit-frame-pointer -fno-strength-reduce  -fno-builtin -finline-functions -s

LOCAL_LDLIBS    := -shared -lz -Wl,--version-script=../libretro/link.T 

include $(BUILD_SHARED_LIBRARY)
