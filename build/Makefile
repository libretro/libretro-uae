include ./makefile_common.mk

PLATFLAGS :=

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

TARGET_NAME := puae

ifeq ($(platform), unix)
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
	LDFLAGS := -lz -lpthread
   SHARED := -shared -Wl,--version-script=$(LIBRETRO)/link.T 
else ifeq ($(platform), osx)
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC -mmacosx-version-min=10.6
   SHARED := -dynamiclib
   PLATFLAGS +=  -DRETRO -DLSB_FIRST -DALIGN_DWORD  -D__LIBRETRO__
else ifeq ($(platform), android)
   CC = arm-linux-androideabi-gcc
   AR = @arm-linux-androideabi-ar
   LD = @arm-linux-androideabi-g++ 
   TARGET := $(TARGET_NAME)_libretro_android.so
   fpic := -fPIC
	LDFLAGS := -lz
   SHARED :=  -Wl,--fix-cortex-a8 -shared -Wl,--version-script=$(LIBRETRO)/link.T -Wl,--no-undefined
   PLATFLAGS += -DANDROID -DRETRO -DAND -DLSB_FIRST -DALIGN_DWORD  -D__LIBRETRO__ -DANDPORT -DARM_OPT_TEST=1
else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_wii.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)   
   ZLIB_DIR = $(LIBUTILS)/zlib/
   CFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN \
	-DWIIPORT=1 -DHAVE_MEMALIGN -DHAVE_ASPRINTF -I$(ZLIB_DIR) -I$(DEVKITPRO)/libogc/include \
	-D__powerpc__ -D__POWERPC__ -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__
   LDFLAGS :=   -lm -lpthread -lc
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD -DWIIPORT
else ifeq ($(platform), ps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   ZLIB_DIR = $(LIBUTILS)/zlib/
   LDFLAGS :=   -lm -lpthread -lc
   CFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN \
	-D__CELLOS_LV2 -DPS3PORT=1 -DHAVE_MEMALIGN -DHAVE_ASPRINTF -I$(ZLIB_DIR) 
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD 
else


ifeq ($(subplatform), 32)
   CC = i586-mingw32msvc-gcc
else
   CC = x86_64-w64-mingw32-gcc
   CFLAGS += -fno-aggressive-loop-optimizations
endif
   PLATFLAGS +=  -DRETRO -DLSB_FIRST -DALIGN_DWORD -DWIN32PORT -DWIN32
   TARGET := $(TARGET_NAME)_libretro.dll
   fpic := -fPIC
   SHARED := -shared -static-libgcc -s -Wl,--version-script=$(LIBRETRO)/link.T -Wl,--no-undefined 
	LDFLAGS := -lm -lz
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -Ofast
endif

DEFINES += -DCPUEMU_0 -DCPUEMU_11 -DCPUEMU_12 -DCPUEMU_20 -DCPUEMU_21 -DCPUEMU_22  -DFPUEMU -DUNALIGNED_PROFITABLE -DAMAX -DAGA -DAUTOCONFIG -DFILESYS -DSUPPORT_THREADS  -DFDI2RAW -DDEBUGGER -DSAVESTATE -DENFORCER -DACTION_REPLAY -DXARCADE -DCPUEMU_31 -DCPUEMU_32  -DCPUEMU_33 -DMMUEMU -DFULLMMU -DDRIVESOUND
#-DSCSIEMU -DSCSIEMU_LINUX_IOCTL -DUSE_SDL -DDRIVESOUND -DBSDSOCKET -DCDTV -DCD32  -DA2091 -DNCR  -DGAYLE
CFLAGS += $(DEFINES) -DRETRO=1 -std=gnu99 

all: $(TARGET)

ifeq ($(platform), wii)
$(TARGET): $(OBJECTS) 
	$(AR) rcs $@ $(OBJECTS) 
else ifeq ($(platform), ps3)
$(TARGET): $(OBJECTS) 
	$(AR) rcs $@ $(OBJECTS) 

else ifeq ($(platform), win)
$(TARGET): $(OBJECTS)
	$(CC) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS)
else
$(TARGET): $(OBJECTS)
	$(CC) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS)

endif

%.o: %.c
	$(CC) $(fpic) $(CFLAGS) $(PLATFLAGS) $(HINCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET) 

.PHONY: clean

