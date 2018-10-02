
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
else ifneq ($(findstring arm,$(shell uname -a)),)
    PLATFLAGS +=  -DARM  -marm
endif
endif

TARGET_NAME := puae

GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
	CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

CORE_DIR  := .
ROOT_DIR  := .

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
	LDFLAGS := -lpthread
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T

# use for raspberry pi
else ifeq ($(platform), rpi)
	   TARGET := $(TARGET_NAME)_libretro.so
	   fpic := -fPIC
		 LDFLAGS := -lpthread
		 PLATFLAGS +=  -DARM  -marm
	   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T

else ifeq ($(platform), osx)
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC -mmacosx-version-min=10.6
   LDFLAGS :=
   SHARED := -dynamiclib
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD

else ifeq ($(platform), android-armv7)
   CC = arm-linux-androideabi-gcc
   AR = @arm-linux-androideabi-ar
   LD = @arm-linux-androideabi-g++
   TARGET := $(TARGET_NAME)_libretro_android.so
   fpic := -fPIC
	LDFLAGS := -lm
   SHARED :=  -Wl,--fix-cortex-a8 -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T -Wl,--no-undefined
   PLATFLAGS += -DANDROID -DRETRO -DAND -DALIGN_DWORD -DA_ZIP

else ifeq ($(platform), android)
   CC = arm-linux-androideabi-gcc
   AR = @arm-linux-androideabi-ar
   LD = @arm-linux-androideabi-g++ 
   TARGET := $(TARGET_NAME)_libretro_android.so
   fpic := -fPIC
	LDFLAGS := 
   SHARED :=  -Wl,--fix-cortex-a8 -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T -Wl,--no-undefined
   PLATFLAGS += -DANDROID -DRETRO -DAND -DALIGN_DWORD -DARM_OPT_TEST=1

else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   ZLIB_DIR = $(LIBUTILS)/zlib/
   CFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN \
	-DHAVE_MEMALIGN -DHAVE_ASPRINTF -I$(ZLIB_DIR) -I$(DEVKITPRO)/libogc/include \
	-D__powerpc__ -D__POWERPC__ -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__
   LDFLAGS :=   -lm -lpthread -lc
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD
	STATIC_LINKING=1
# Nintendo Wii U
else ifeq ($(platform), wiiu)
       TARGET := $(TARGET_NAME)_libretro_$(platform).a
       CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
       CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
       AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
       COMMONFLAGS += -DGEKKO -DWIIU -DHW_RVL -mwup -mcpu=750 -meabi -mhard-float -D__POWERPC__ -D__ppc__ -DMSB_FIRST -DWORDS_BIGENDIAN=1
       COMMONFLAGS += -U__INT32_TYPE__ -U __UINT32_TYPE__ -D__INT32_TYPE__=int -DC68K_BIG_ENDIAN
       COMMONFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN 
       CFLAGS += $(COMMONFLAGS) -I$(DEVKITPRO)/portlibs/ppc/include -I$(CORE_DIR)/wiiu-deps
       HAVE_COMPAT = 1
       MSB_FIRST = 1
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD
	STATIC_LINKING=1
STATIC_LINKING_LINK=1
else ifeq ($(platform), ps3)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   ZLIB_DIR = $(LIBUTILS)/zlib/
   LDFLAGS :=   -lm -lpthread -lc
   CFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN  -DBYTE_ORDER=BIG_ENDIAN \
	-D__CELLOS_LV2__ -DHAVE_MEMALIGN -DHAVE_ASPRINTF -I$(ZLIB_DIR)
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD
	STATIC_LINKING=1
else ifeq ($(platform), emscripten)
   TARGET := $(TARGET_NAME)_libretro_$(platform).bc
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD
   CFLAGS    +=  -DHAVE_MEMALIGN -DHAVE_ASPRINTF -I$(ZLIB_DIR)
	STATIC_LINKING=1

else

ifneq ($(subplatform), 32)
   CFLAGS += -fno-aggressive-loop-optimizations
endif
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD -DWIN32
   TARGET := $(TARGET_NAME)_libretro.dll
   fpic := -fPIC
   SHARED := -shared -static-libgcc -s -Wl,--version-script=$(CORE_DIR)/libretro/link.T -Wl,--no-undefined
	LDFLAGS := -lm
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O2
endif

DEFINES += -DCPUEMU_0 -DCPUEMU_11 -DCPUEMU_12 -DCPUEMU_20 -DCPUEMU_21 -DCPUEMU_22  -DFPUEMU -DUNALIGNED_PROFITABLE -DAMAX -DAGA -DAUTOCONFIG -DFILESYS -DSUPPORT_THREADS  -DFDI2RAW -DDEBUGGER -DSAVESTATE -DENFORCER -DACTION_REPLAY -DXARCADE -DCPUEMU_31 -DCPUEMU_32  -DCPUEMU_33 -DMMUEMU -DFULLMMU
#-DSCSIEMU -DSCSIEMU_LINUX_IOCTL -DUSE_SDL -DDRIVESOUND -DBSDSOCKET -DCDTV -DCD32  -DA2091 -DNCR  -DGAYLE
DEFINES += -D__LIBRETRO__
CFLAGS += $(DEFINES) -DRETRO=1 -DINLINE="inline" -std=gnu99

include Makefile.common

OBJECTS += $(SOURCES_C:.c=.o) $(SOURCES_CXX:.cpp=.o) $(SOURCES_ASM:.S=.o)

INCDIRS := $(EXTRA_INCLUDES) $(INCFLAGS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING_LINK),1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CC) $(fpic) $(SHARED) $(INCDIRS) -o $@ $(OBJECTS) $(LDFLAGS)
endif

%.o: %.c
	$(CC) $(fpic) $(CFLAGS) $(PLATFLAGS) $(INCDIRS) -c -o $@ $<

%.o: %.S
	$(CC_AS) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

