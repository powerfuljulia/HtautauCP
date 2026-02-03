Target  = Htautau

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs)

PYTHIA8_CONFIG = $(PYTHIA8)/bin/pythia8-config
PYTHIAFLAGS = $(shell $(PYTHIA8_CONFIG) --cxxflags)
PYTHIALIBS  = $(shell $(PYTHIA8_CONFIG) --libs)

CXX = g++
CXXFLAGS = -g $(ROOTFLAGS) $(PYTHIAFLAGS)
LDFLAGS  = $(ROOTLIBS) $(PYTHIALIBS)

all: $(Target)

$(Target): Htautau.cxx Htautau.h
	$(CXX) $(CXXFLAGS) -o $@ Htautau.cxx $(LDFLAGS)

clean:
	rm -f $(Target) *.o
