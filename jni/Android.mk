LOCAL_PATH := $(call my-dir)

CORE_DIR := $(LOCAL_PATH)/..

SOURCES_C :=

# 7zip
CFLAGS += -DHAVE_7ZIP -D_7ZIP_ST

# CHD
HAVE_CHD = 1

include $(CORE_DIR)/Makefile.common

COREFLAGS := -DANDROID -D__LIBRETRO__ $(CFLAGS) $(INCFLAGS) -I$(LOCAL_PATH)/include

include $(CLEAR_VARS)
LOCAL_MODULE    := retro
LOCAL_SRC_FILES := $(SOURCES_C)
LOCAL_CFLAGS    := -std=gnu99 $(COREFLAGS)
LOCAL_LDFLAGS   := -Wl,-version-script=$(CORE_DIR)/libretro/link.T
LOCAL_LDLIBS    := -lz
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true
LOCAL_ARM_MODE  := arm
include $(BUILD_SHARED_LIBRARY)
