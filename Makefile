.PHONY: all clean

TARGET = soulfu
SRC = source/soulfu.c
OBJ = $(SRC:.c=.o)
LIB = -lm -lSDL -lSDL_net -lGL -logg -ljpeg -lvorbis
FLAGS = -m32 -g

ifdef DEVTOOL
FLAGS += -DDEVTOOL
endif

all: $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ)

$(TARGET): $(OBJ)
	gcc $(FLAGS) -o $@ -Isource $(LIB) $(OBJ)

%.o: %.c
	gcc $(FLAGS) -c -o $@ -Isource $(LIB) $<
