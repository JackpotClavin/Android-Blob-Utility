
BUILD_WITH_READLINE := false
VARIABLES_PROVIDED := false

CC = gcc
CFLAGS += -Wall -Wextra

ifeq ($(BUILD_WITH_READLINE), true)
	CFLAGS += -DUSE_READLINE
	LDFLAGS += -lreadline
endif

ifeq ($(VARIABLES_PROVIDED), true)
	CFLAGS += -DVARIABLES_PROVIDED
endif

OBJECTS = android-blob-utility.o
SOURCE = android-blob-utility.c
MODULE = android-blob-utility


android-blob-utility: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(MODULE) $(LDFLAGS)

all: android-blob-utility

clean:
	-rm -f $(OBJECTS) $(MODULE)

