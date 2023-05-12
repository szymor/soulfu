.PHONY: all clean pack

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

all: $(TARGET) $(DATAFILE)

clean:
	rm -rf $(TARGET) $(OBJ) $(DATAFILE)

pack: $(DATAFILE)

$(DATAFILE): | $(DATADIR) $(SDP)
	$(SSC) -c -i $(DATADIR)
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
