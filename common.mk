.PHONY: data

SDP = sdp/sdp
SLC = slc/slc
SSC = ssc/ssc

DATAFILE = datafile.sdf
DATADIR = datadir

LANTXT = $(wildcard language/*.txt)
LANFILES = $(patsubst language/%.txt,$(DATADIR)/%.LAN, $(LANTXT))

SCRIPTSRC = $(wildcard $(DATADIR)/*.SRC)
SCRIPTRUN = $(SCRIPTSRC:.SRC=.RUN)

DATAEXT = DAT DDD JPG MUS OGG PCX SRC SRF TXT
DATADEP = $(foreach ext,$(DATAEXT),$(wildcard $(DATADIR)/*.$(ext))) $(LANFILES) $(SCRIPTRUN)

data: $(DATAFILE)

clean:
	rm -rf $(TARGET) $(TARGET).exe $(DATAFILE) $(LANFILES) $(SCRIPTRUN) resource.res

# TBD: ssc should compile only things that changed since last build
$(DATAFILE): $(DATADEP) | $(SDP)
	$(SDP) -p -n -i $(DATADIR) -o $(DATAFILE)

$(DATADIR)/%.LAN: language/%.txt | $(SLC)
	$(SLC) --to-lan $< -o $@

$(SCRIPTRUN): $(SCRIPTSRC) | $(SSC)
	$(SSC) -c -i $(DATADIR)

$(SDP): | sdp
	make -C sdp

$(SLC): | slc
	make -C slc

$(SSC): | ssc
	make -C ssc
