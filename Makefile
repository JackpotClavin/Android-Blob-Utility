
BUILD_WITH_READLINE := false
# Change value below to match your /system dump's SDK version.
# See: developer.android.com/guide/topics/manifest/uses-sdk-element.html#ApiLevels
SYSTEM_DUMP_SDK_VERSION := 19 # Android KitKat

CC = gcc
CFLAGS += -Wall -Wextra

ifeq ($(BUILD_WITH_READLINE), true)
	CFLAGS += -DUSE_READLINE
	LDFLAGS += -lreadline
endif

CFLAGS += -DSYSTEM_DUMP_SDK_VERSION=$(SYSTEM_DUMP_SDK_VERSION)
OBJECTS = android-blob-utility.o
SOURCE = android-blob-utility.c
MODULE = android-blob-utility


android-blob-utility: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(MODULE) $(LDFLAGS)

all: android-blob-utility

clean:
	-rm -f $(OBJECTS) $(MODULE)

