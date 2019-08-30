.PHONY: all clean

TARGET = soulfu
SRC = source/soulfu.c
OBJ = $(SRC:.c=.o)
LIB = -lm -lSDL -lSDL_net -lGL -logg -ljpeg -lvorbis

all: $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ)

$(TARGET): $(OBJ)
	gcc -m32 -g -o $@ -Isource $(LIB) $(OBJ)

%.o: %.c
	gcc -m32 -g -c -o $@ -Isource $(LIB) $<
