.PHONY: all clean pack scripts

SDP = sdp/sdp
SLC = slc/slc
SSC = ssc/ssc

TARGET = soulfu
DATAFILE = datafile.sdf
DATADIR = datadir
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

pack: $(DATAFILE)

scripts: | $(DATADIR) $(SSC)
	$(SSC) -c -i $(DATADIR)

$(DATAFILE): scripts | $(DATADIR) $(SDP)
	$(SDP) -p -i $(DATADIR) -o $(DATAFILE)

$(SDP): | sdp
	make -C sdp

$(SLC): | slc
	make -C slc

$(SSC): | ssc
	make -C ssc

$(TARGET): $(OBJ)
	gcc $(FLAGS) -o $@ -Isource $(LIB) $(OBJ)

%.o: %.c
	gcc $(FLAGS) -c -o $@ -Isource $(LIB) $<
