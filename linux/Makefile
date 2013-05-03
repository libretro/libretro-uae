platform = unix

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

ifeq ($(platform), unix)
   CC = gcc
   TARGET := libretro-euae.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=../libretro/link.T -Wl,--no-undefined -fPIC
else ifeq ($(platform), osx)
   TARGET := libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
else
   CC = gcc
   TARGET := retro-euae.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=../libretro/link.T -Wl,--no-undefined
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O3
endif

EMU = ../e-uae/src
#GLUE = ../minivmac

CORE_SRCS := $(EMU)/main.o $(EMU)/newcpu.o $(EMU)/fpp.o $(EMU)/memory.o $(EMU)/custom.o $(EMU)/serial.o $(EMU)/cia.o \
	$(EMU)/blitter.o $(EMU)/blittable.o $(EMU)/blitfunc.o $(EMU)/ersatz.o $(EMU)/keybuf.o $(EMU)/expansion.o \
	$(EMU)/cfgfile.o $(EMU)/inputdevice.o $(EMU)/gfxutil.o $(EMU)/audio.o $(EMU)/drawing.o \
	$(EMU)/identify.o $(EMU)/disk.o $(EMU)/savestate.o $(EMU)/uaelib.o $(EMU)/fdi2raw.o \
	$(EMU)/hotkeys.o $(EMU)/enforcer.o  $(EMU)/missing.o \
	$(EMU)/readcpu.o $(EMU)/writelog.o $(EMU)/unzip.o $(EMU)/zfile.o $(EMU)/sinctable.o

CORE_SRCS += $(EMU)/hardfile.o $(EMU)/hardfile_unix.o $(EMU)/scsi-none.o $(EMU)/fsusage.o  $(EMU)/uaecrc32.o \
	$(EMU)/events.o $(EMU)/misc.o 

#PPU_SRCS += blkdev.o scsiemul.o

# cpu code
CORE_SRCS +=   $(EMU)/cpustbl.o $(EMU)/cpudefs.o  $(EMU)/cpuemu_0.o 
CORE_SRCS += $(EMU)/cpuemu_5.o $(EMU)/cpuemu_6.o 

CORE_SRCS += $(EMU)/ar.o $(EMU)/autoconf.o $(EMU)/traps.o $(EMU)/filesys.o $(EMU)/filesys_unix.o $(EMU)/fsdb.o $(EMU)/fsdb_unix.o	\
		$(EMU)/native2amiga.o $(EMU)/uaeexe.o $(EMU)/bsdsocket.o $(EMU)/driveclick.o
# zlib
#PPU_SRCS += zlib/adler32.o zlib/compress.o zlib/crc32.o zlib/deflate.o \
#	zlib/uncompr.o  zlib/trees.o zlib/inflate.o zlib/infback.o \
#	zlib/inftrees.o zlib/inffast.o zlib/zutil.o

#dms
CORE_SRCS += $(EMU)/dms/crc_csum.o $(EMU)/dms/getbits.o $(EMU)/dms/maketbl.o $(EMU)/dms/pfile.o $(EMU)/dms/tables.o \
	$(EMU)/dms/u_deep.o $(EMU)/dms/u_heavy.o $(EMU)/dms/u_init.o $(EMU)/dms/u_medium.o $(EMU)/dms/u_quick.o \
	$(EMU)/dms/u_rle.o
# libretro port code
CORE_SRCS += $(EMU)/machdep/support.o $(EMU)/osdep/gui.o $(EMU)/osdep/retroglue.o $(EMU)/sounddep/sound.o \
	$(EMU)/osdep/retromenu.o

BUILD_APP =  $(CORE_SRCS) 

HINCLUDES := -I./$(EMU) -I./$(EMU)/include -I../libretro 

OBJECTS := $(BUILD_APP) ../libretro/libretro-euae.o ../libretro/euae-mapper.o ../libretro/vkbd.o \
	../libretro/graph.o ../libretro/diskutils.o ../libretro/fontmsx.o  
	
DEFINES += -DFPUEMU -DCPUEMU_0 -DCPUEMU_5 -DCPUEMU_6 

DEFINES +=  -DAGA -DFDI2RAW -DFILESYS -DAUTOCONFIG -DSAVESTATE -DENFORCER -DACTION_REPLAY -DSUPPORT_THREADS=0 

CFLAGS += $(DEFINES) -DRETRO=1 -std=gnu99  -O3 -finline-functions -funroll-loops  -fsigned-char  \
	-Wno-strict-prototypes -ffast-math -fomit-frame-pointer -fno-strength-reduce  -fno-builtin -finline-functions -s -fPIC

CXXFLAGS  +=	$(CFLAGS) -std=gnu++0x
CPPFLAGS += $(CFLAGS)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) -lm -lpthread -lz 
	
%.o: %.c
	$(CC) $(CFLAGS) $(HINCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

