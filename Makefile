
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

MODULE = android-blob-utility


all: $(MODULE)

$(MODULE): $(MODULE).h

clean:
	-rm -f $(MODULE)

