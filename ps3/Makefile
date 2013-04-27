
PS3DEV := /usr/local/ps3dev
CELL_SDK := /root/.wine/drive_c/usr/local/cell

platform = ps3

ifeq ($(platform), ps3)
   TARGET := euae_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   CXX = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-g++.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   SHARED :=  
   ZLIB_DIR = ../utils/zlib/
   ZLIB_OBJECTS	:= $(ZLIB_DIR)adler32.o $(ZLIB_DIR)compress.o $(ZLIB_DIR)crc32.o $(ZLIB_DIR)deflate.o \
		$(ZLIB_DIR)gzclose.o $(ZLIB_DIR)gzlib.o $(ZLIB_DIR)gzread.o\
		$(ZLIB_DIR)gzwrite.o $(ZLIB_DIR)infback.o $(ZLIB_DIR)inffast.o $(ZLIB_DIR)inflate.o \
		$(ZLIB_DIR)trees.o $(ZLIB_DIR)inftrees.o $(ZLIB_DIR)uncompr.o $(ZLIB_DIR)uncompr.o $(ZLIB_DIR)zutil.o 
   CFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN -DWORDS_BIGENDIAN=1\
	 -D__CELLOS_LV2 -DPS3PORT=1 -D__PS3__ -I$(ZLIB_DIR)

endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O3
endif

EMU = ../e-uae/src

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

#dms
CORE_SRCS += $(EMU)/dms/crc_csum.o $(EMU)/dms/getbits.o $(EMU)/dms/maketbl.o $(EMU)/dms/pfile.o $(EMU)/dms/tables.o \
	$(EMU)/dms/u_deep.o $(EMU)/dms/u_heavy.o $(EMU)/dms/u_init.o $(EMU)/dms/u_medium.o $(EMU)/dms/u_quick.o \
	$(EMU)/dms/u_rle.o
# libretro port code
CORE_SRCS += $(EMU)/machdep/support.o $(EMU)/osdep/gui.o $(EMU)/osdep/retroglue.o $(EMU)/sounddep/sound.o \
	$(EMU)/osdep/retromenu.o

BUILD_APP =  $(CORE_SRCS) $(ZLIB_OBJECTS) 

HINCLUDES := -I./$(EMU) -I./$(EMU)/include -I../libretro  

OBJECTS := $(BUILD_APP) ../libretro/libretro-euae.o ../libretro/euae-mapper.o ../libretro/vkbd.o \
	../libretro/graph.o ../libretro/diskutils.o ../libretro/fontmsx.o  
	
DEFINES += -DFPUEMU -DCPUEMU_0 -DCPUEMU_5 -DCPUEMU_6 

DEFINES +=  -DAGA -DFDI2RAW -DFILESYS -DAUTOCONFIG -DSAVESTATE -DENFORCER -DACTION_REPLAY -DSUPPORT_THREADS=0 

CFLAGS += $(DEFINES) -DRETRO=1 -std=gnu99  -O3 -finline-functions -ffast-math -fomit-frame-pointer -s -fPIC
#-funroll-loops  -fsigned-char  \
#	-Wno-strict-prototypes -ffast-math -fomit-frame-pointer -fno-strength-reduce  -fno-builtin -finline-functions -s -fPIC

CXXFLAGS  +=	$(CFLAGS) -std=gnu++0x
CPPFLAGS += $(CFLAGS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
#	$(CC) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) -lm -lpthread -lz 
	$(AR) rcs $@ $(OBJECTS) 
%.o: %.c
	$(CC) $(CFLAGS) $(HINCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

