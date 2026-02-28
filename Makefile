PKG ?= pkg-config

TARGET = soulfu
SRC = source/soulfu.c
DEP = $(wildcard source/*.c source/*.h)
LDLIBS = -lm $(shell $(PKG) --libs sdl2 ogg libjpeg vorbis gl)
FLAGS =

debug: all
debug: FLAGS += -g -DDEVTOOL -rdynamic
.PHONY: debug

release: all
release: FLAGS += -DSDF_PATH="\"/usr/share/soulfu/datafile.sdf\""
.PHONY: release

all: $(TARGET) data
.PHONY: all

$(TARGET): $(DEP)
	$(CC) $(FLAGS) -o $@ -Isource $(SRC) $(LDLIBS)

include common.mk
