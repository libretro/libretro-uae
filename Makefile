
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
	LDFLAGS += -lpthread
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T

# use for raspberry pi
else ifeq ($(platform), rpi)
	   TARGET := $(TARGET_NAME)_libretro.so
	   fpic := -fPIC
		 LDFLAGS := -lpthread
		 PLATFLAGS +=  -DARM  -marm
	   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T

# Classic Platforms ####################
# Platform affix = classic_<ISA>_<µARCH>
# Help at https://modmyclassic.com/comp

# (armv7 a7, hard point, neon based) ### 
# NESC, SNESC, C64 mini 
else ifeq ($(platform), classic_armv7_a7)
	TARGET := $(TARGET_NAME)_libretro.so
	fpic := -fPIC
    SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T  -Wl,--no-undefined
    LDFLAGS += -lm -lpthread
    CFLAGS += -Ofast -DARM \
	-flto=4 -fwhole-program -fuse-linker-plugin \
	-fdata-sections -ffunction-sections -Wl,--gc-sections \
	-fno-stack-protector -fno-ident -fomit-frame-pointer \
	-falign-functions=1 -falign-jumps=1 -falign-loops=1 \
	-fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops \
	-fmerge-all-constants -fno-math-errno \
	-marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
	CXXFLAGS += $(CFLAGS)
	CPPFLAGS += $(CFLAGS)
	ASFLAGS += $(CFLAGS)
	ifeq ($(shell echo `$(CC) -dumpversion` "< 4.9" | bc -l), 1)
	  CFLAGS += -march=armv7-a
	else
	  CFLAGS += -march=armv7ve
	  # If gcc is 5.0 or later
	  ifeq ($(shell echo `$(CC) -dumpversion` ">= 5" | bc -l), 1)
	    LDFLAGS += -static-libgcc -static-libstdc++
	  endif
	endif
#######################################
# (armv8 a35, hard point, neon based) ###
# PlayStation Classic
else ifeq ($(platform), classic_armv8_a35)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T  -Wl,--no-undefined
   LDFLAGS += -lm -lrt -lpthread -ldl
   CFLAGS += -Ofast -DARM -flto=4 \
           -fmerge-all-constants -fno-math-errno -march=armv8-a \
           -marm -mtune=cortex-a35 -mfpu=neon-fp-armv8 -mfloat-abi=hard
   LDFLAGS += -static-libgcc -static-libstdc++

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

# CTR(3DS)
else ifeq ($(platform), ctr)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITARM)/bin/arm-none-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITARM)/bin/arm-none-eabi-g++$(EXE_EXT)
   AR = $(DEVKITARM)/bin/arm-none-eabi-ar$(EXE_EXT)
   CFLAGS += -DARM11 -D_3DS -march=armv6k -mtune=mpcore -mfloat-abi=hard
   PLATFLAGS += -DRETRO -DALIGN_DWORD
   STATIC_LINKING=1
   STATIC_LINKING_LINK=1

# Nintendo Game Cube / Wii / WiiU
else ifneq (,$(filter $(platform), ngc wii wiiu))
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   COMMONFLAGS += -DGEKKO -mcpu=750 -meabi -mhard-float -D__POWERPC__ -D__ppc__ -DMSB_FIRST -DWORDS_BIGENDIAN=1
   COMMONFLAGS += -U__INT32_TYPE__ -U __UINT32_TYPE__ -D__INT32_TYPE__=int -DC68K_BIG_ENDIAN
   COMMONFLAGS += -DSDL_BYTEORDER=SDL_BIG_ENDIAN -DBYTE_ORDER=BIG_ENDIAN
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD
   STATIC_LINKING=1
   STATIC_LINKING_LINK=1
   ifneq (,$(findstring wiiu,$(platform)))
      COMMONFLAGS += -DWIIU -DHW_RVL
   else ifneq (,$(findstring wii,$(platform)))
      COMMONFLAGS += -DHW_RVL -mrvl
   else ifneq (,$(findstring ngc,$(platform)))
      COMMONFLAGS += -DHW_DOL -mrvl
   endif
   CFLAGS += $(COMMONFLAGS) -I$(DEVKITPRO)/portlibs/ppc/include -I$(CORE_DIR)/wiiu-deps

# Nintendo Switch (libnx)
else ifeq ($(platform), libnx)
   include $(DEVKITPRO)/libnx/switch_rules
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CFLAGS += -D__SWITCH__ -DHAVE_LIBNX -I$(DEVKITPRO)/libnx/include/ -specs=$(DEVKITPRO)/libnx/switch.specs
   CFLAGS += -march=armv8-a -mtune=cortex-a57 -mtp=soft -mcpu=cortex-a57+crc+fp+simd -ffast-math -fPIE -ffunction-sections
   PLATFLAGS += -DRETRO -DALIGN_DWORD -DARM
   STATIC_LINKING=1
   STATIC_LINKING_LINK=1

# Vita
else ifeq ($(platform), vita)
   TARGET := $(TARGET_NAME)_libretro_vita.a
   CC = arm-vita-eabi-gcc$(EXE_EXT)
   CXX = arm-vita-eabi-g++$(EXE_EXT)
   AR = arm-vita-eabi-ar$(EXE_EXT)
   COMMONFLAGS += -U__INT32_TYPE__ -U __UINT32_TYPE__ -D__INT32_TYPE__=int
   COMMONFLAGS += -DHAVE_STRTOUL -DVITA
   CFLAGS += $(COMMONFLAGS)
   PLATFLAGS += -DRETRO -DALIGN_DWORD -DARM
   STATIC_LINKING = 1
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

else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   COMMONFLAGS += -DHAVE_POSIX_MEMALIGN=1 -marm
   fpic := -fPIC
   SHARED := -dynamiclib
   COMMONFLAGS += -DIOS
   ifeq ($(IOSSDK),)
      IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
   endif
   ifeq ($(platform),ios-arm64)
      CC = cc -arch arm64 -isysroot $(IOSSDK)
      CXX = c++ -arch arm64 -isysroot $(IOSSDK)
      PLATFLAGS += -Wno-error=implicit-function-declaration
   else
      CC = cc -arch armv7 -isysroot $(IOSSDK)
      CXX = c++ -arch armv7 -isysroot $(IOSSDK)
   endif
   ifeq ($(platform),$(filter $(platform),ios9 ios-arm64))
      CC += -miphoneos-version-min=8.0
      COMMONFLAGS += -miphoneos-version-min=8.0
   else
      CC += -miphoneos-version-min=5.0
      COMMONFLAGS += -miphoneos-version-min=5.0
   endif
   CFLAGS += $(COMMONFLAGS)
   PLATFLAGS += -DRETRO -DALIGN_DWORD -DARM

else ifeq ($(platform), tvos-arm64)
   TARGET := $(TARGET_NAME)_libretro_tvos.dylib
   COMMONFLAGS += -DHAVE_POSIX_MEMALIGN=1 -marm
   fpic := -fPIC
   SHARED := -dynamiclib
   COMMONFLAGS += -DIOS
   PLATFLAGS += -Wno-error=implicit-function-declaration
   CFLAGS += $(COMMONFLAGS)
   PLATFLAGS += -DRETRO -DALIGN_DWORD -DARM
   ifeq ($(IOSSDK),)
      IOSSDK := $(shell xcodebuild -version -sdk appletvos Path)
   endif

# ARM
else ifneq (,$(findstring armv,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.so
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/libretro/link.T  -Wl,--no-undefined
   fpic := -fPIC
   ifneq (,$(findstring cortexa8,$(platform)))
      CFLAGS += -marm -mcpu=cortex-a8
   else ifneq (,$(findstring cortexa7,$(platform)))
      CFLAGS += -marm -mcpu=cortex-a7
   else ifneq (,$(findstring cortexa9,$(platform)))
      CFLAGS += -marm -mcpu=cortex-a9
   endif
   CFLAGS += -marm
   ifneq (,$(findstring neon,$(platform)))
      CFLAGS += -mfpu=neon
   endif
   ifneq (,$(findstring softfloat,$(platform)))
      CFLAGS += -mfloat-abi=softfp
   else ifneq (,$(findstring hardfloat,$(platform)))
      CFLAGS += -mfloat-abi=hard
   endif
   PLATFLAGS += -DRETRO -DALIGN_DWORD -DARM
else

ifneq ($(subplatform), 32)
   CFLAGS += -fno-aggressive-loop-optimizations
endif
   PLATFLAGS +=  -DRETRO -DALIGN_DWORD -DWIN32
   TARGET := $(TARGET_NAME)_libretro.dll
   fpic := -fPIC
   SHARED := -shared -static-libgcc -s -Wl,--version-script=$(CORE_DIR)/libretro/link.T -Wl,--no-undefined
	LDFLAGS += -lm
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O2
endif

DEFINES += -DCPUEMU_0 -DCPUEMU_11 -DCPUEMU_12 -DCPUEMU_20 -DCPUEMU_21 -DCPUEMU_22  -DFPUEMU -DUNALIGNED_PROFITABLE -DAMAX -DAGA -DAUTOCONFIG -DFILESYS -DSUPPORT_THREADS  -DFDI2RAW -DDEBUGGER -DSAVESTATE -DACTION_REPLAY -DCPUEMU_31 -DCPUEMU_32  -DCPUEMU_33 -DMMUEMU -DFULLMMU -DDRIVESOUND
# -DENFORCER -DXARCADE -DSCSIEMU -DSCSIEMU_LINUX_IOCTL -DUSE_SDL -DBSDSOCKET -DCDTV -DCD32  -DA2091 -DNCR  -DGAYLE
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

