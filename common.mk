.PHONY: data

SDP = sdp/sdp
SLC = slc/slc
SSC = ssc/ssc

DATAFILE = datafile.sdf
DATADIR = datadir

LANTXT = $(wildcard language/*.txt)
LANFILES = $(patsubst language/%.txt,$(DATADIR)/%.LAN, $(LANTXT))

DATAEXT = DAT DDD JPG LAN MUS OGG PCX SRC SRF TXT
DATADEP = $(foreach ext,$(DATAEXT),$(wildcard $(DATADIR)/*.$(ext))) $(LANFILES)

SCRIPTSRC = $(wildcard $(DATADIR)/*.SRC)
SCRIPTRUN = $(SCRIPTSRC:.SRC=.RUN)

data: $(DATAFILE)

# resource.res built by mingw only
clean:
	rm -rf $(TARGET) $(DATAFILE) $(LANFILES) $(SCRIPTRUN) resource.res

# TBD: ssc should compile only things that changed since last build
$(DATAFILE): $(SCRIPTRUN) $(DATADEP) | $(DATADIR) $(SSC) $(SDP)
	$(SDP) -p -n -i $(DATADIR) -o $(DATAFILE)

$(DATADIR)/%.LAN: language/%.txt
	$(SLC) --to-lan $< -o $@

$(SCRIPTRUN): $(SCRIPTSRC)
	$(SSC) -c -i $(DATADIR)

$(SDP): | sdp
	make -C sdp

$(SLC): | slc
	make -C slc

$(SSC): | ssc
	make -C ssc
