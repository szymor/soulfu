PKG ?= pkg-config

TARGET = soulfu
SRC = source/soulfu.c
DEP = $(wildcard source/*.c source/*.h)
LDLIBS = -lm $(shell $(PKG) --libs sdl SDL_net ogg libjpeg vorbis gl)
FLAGS = -m32

debug: all
debug: FLAGS += -g -DDEVTOOL
.PHONY: debug

release: all
release: FLAGS += -DSDF_PATH="\"/usr/share/soulfu/datafile.sdf\""
.PHONY: release

all: $(TARGET) data
.PHONY: all

$(TARGET): $(DEP)
	$(CC) $(FLAGS) -o $@ -Isource $(SRC) $(LDLIBS)

include common.mk
