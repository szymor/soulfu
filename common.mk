.PHONY: data

SDP = sdp/sdp
SLC = slc/slc
SSC = ssc/ssc

DATAFILE = datafile.sdf
DATADIR = datadir
# do not pack RUN files
DATAEXT = DAT DDD JPG LAN MUS OGG PCX SRC SRF TXT
DATADEP = $(foreach ext,$(DATAEXT),$(wildcard $(DATADIR)/*.$(ext)))

data: $(DATAFILE)

# TBD: ssc should compile only things that changed since last build
$(DATAFILE): $(DATADEP) | $(DATADIR) $(SSC) $(SDP)
	$(SSC) -c -i $(DATADIR)
	$(SDP) -p -n -i $(DATADIR) -o $(DATAFILE)

$(SDP): | sdp
	make -C sdp

$(SLC): | slc
	make -C slc

$(SSC): | ssc
	make -C ssc