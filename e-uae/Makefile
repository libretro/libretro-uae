.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT must be set in the environment.")
endif

include $(PSL1GHT)/ppu_rules

RELEASE		:=	r8

TARGET		:=	e-uae-0.8.29-ps3_$(RELEASE)
BUILD		:=	build
SOURCES		:=	src
BUILD_DIRS  :=  dms machdep osdep sounddep
INCLUDES    :=	src src/include src/zlib
DATA		:=	data

TITLE		:=	E-UAE 0.8.29-WIP4 $(RELEASE)
APPID		:=	EUAE00829
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000
PKGFILES	:=	release

ICON0		:=	ICON0.PNG

CFLAGS_USER	+= -std=gnu99 
CXXFLAGS_USER	+= -Wall

DEFINES := -D__PS3__ 
DEFINES += -DFPUEMU -DCPUEMU_0 -DCPUEMU_5 -DCPUEMU_6 
DEFINES +=  -DAGA -DFDI2RAW -DFILESYS -DAUTOCONFIG -DSAVESTATE -DENFORCER -DACTION_REPLAY -DSUPPORT_THREADS -DDRIVESOUND
#CPPFLAGS =  -I/usr/include  -DFPUEMU -DCPUEMU_0 -DCPUEMU_5 -DCPUEMU_6 -DUNALIGNED_PROFITABLE -DAGA -DAUTOCONFIG -DFILESYS -DBSDSOCKET -DSUPPORT_THREADS -DFDI2RAW -DDEBUGGER -DSAVESTATE -DENFORCER -DACTION_REPLAY  -DSHM_SUPPORT_LINKS=1

ifeq ("$(log)","true")
DEFINES		+=  -DENABLE_LOG
CFLAGS_USER		+=  -O0
else
CFLAGS_USER		+=  -O3
endif



#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		=	-Wall -mcpu=cell $(MACHDEP) $(DEFINES)  $(INCLUDE) $(CFLAGS_USER)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lgcm_sys -lrsx -lsysutil -lio -lsysmodule -laudio -lcairo -lm -lnet -lfreetype -lz -lpixman-1 -lrt -llv2

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
#CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
#CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
#sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
#SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
#BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# specify the list of object files for our project
#---------------------------------------------------------------------------------
PPU_SRCS := main.c newcpu.c fpp.c memory.c custom.c serial.c cia.c \
	blitter.c blittable.c blitfunc.c ersatz.c keybuf.c expansion.c \
	cfgfile.c inputdevice.c gfxutil.c audio.c drawing.c \
	identify.c disk.c savestate.c uaelib.c fdi2raw.c \
	hotkeys.c enforcer.c  missing.c \
	readcpu.c writelog.c unzip.c zfile.c sinctable.c

PPU_SRCS += hardfile.c hardfile_unix.c scsi-none.c fsusage.c  crc32.c \
	events.c misc.c 

#PPU_SRCS += blkdev.c scsiemul.c

# cpu code
PPU_SRCS +=  cpuemu_0.c  cpustbl.c cpudefs.c 
PPU_SRCS += cpuemu_5.c cpuemu_6.c
	

PPU_SRCS += ar.c autoconf.c traps.c filesys.c filesys_unix.c fsdb.c fsdb_unix.c	\
		native2amiga.c uaeexe.c bsdsocket.c driveclick.c

# zlib
#PPU_SRCS += zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c \
#	zlib/uncompr.c  zlib/trees.c zlib/inflate.c zlib/infback.c \
#	zlib/inftrees.c zlib/inffast.c zlib/zutil.c

#dms
PPU_SRCS += dms/crc_csum.c dms/getbits.c dms/maketbl.c dms/pfile.c dms/tables.c \
	dms/u_deep.c dms/u_heavy.c dms/u_init.c dms/u_medium.c dms/u_quick.c \
	dms/u_rle.c

# ps3 port code
PPU_SRCS += machdep/support.c osdep/gui.c osdep/cell.c osdep/ps3display.c \
	osdep/input.c osdep/ps2virtkb.c osdep/ps2options.c osdep/ps3hwkb.c \
	osdep/ps2fselect.c osdep/ps3main.c  osdep/log.c  osdep/ps3thumbnail.c sounddep/sound.c

CFILES	:= 	$(PPU_SRCS)

#---------------------------------------------------------------------------------
# extra directories that need to be created before building 
#---------------------------------------------------------------------------------
DIRS_TO_CREATE := $(addsuffix /dummy.dirtocreate,$(BUILD_DIRS));
DIR_TO_CREATE = $(BUILD)/$(@D)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD): $(DIRS_TO_CREATE)
	
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self


#---------------------------------------------------------------------------------
# implicit rule to create a directory within the build directory
#---------------------------------------------------------------------------------
%.dirtocreate :
	@[ -d $(DIR_TO_CREATE) ] || mkdir -p $(DIR_TO_CREATE)

else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------








