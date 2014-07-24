# 
# Copyright 2014 JackpotClavin
#
# Android Blob Utility
#

# Change value below to match your /system dump's SDK version.
# See: developer.android.com/guide/topics/manifest/uses-sdk-element.html#ApiLevels
SYSTEM_DUMP_SDK_VERSION := 19 # Android KitKat

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := android-blob-utility.c

LOCAL_CFLAGS += -DSYSTEM_DUMP_SDK_VERSION=$(SYSTEM_DUMP_SDK_VERSION)

LOCAL_MODULE := android-blob-utility

include $(BUILD_HOST_EXECUTABLE)
