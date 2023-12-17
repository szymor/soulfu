.PHONY: all

PKG ?= pkg-config

TARGET = soulfu
SRC = source/soulfu.c
DEP = $(wildcard source/*.c source/*.h)
LDLIBS = -lm $(shell $(PKG) --libs sdl SDL_net ogg libjpeg vorbis gl)
FLAGS = -g -m32 -DDEVTOOL

all: $(TARGET) data

$(TARGET): $(DEP)
	$(CC) $(FLAGS) -o $@ -Isource $(SRC) $(LDLIBS)

include common.mk
