ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --glibs) -lMathMore -lRooFit -lRooStats -lRooFitCore -lFoam -lMinuit -lTreePlayer -lHistFactory -lXMLIO

CXX           = g++
CXXFLAGS      = -O0 -Wall -fPIC -g $(ROOTCFLAGS) 
CXXLIBS       = $(ROOTLIBS)

default : Analyser

Analyser: Analyser.o TreeReader.o AtlasStyle.o 
	$(CXX) -o $@ $^ $(CXXLIBS)

Analyser.o: Analyser.cxx
	$(CXX) -c $< $(CXXFLAGS)

TreeReader.o: TreeReader.cxx TreeReader.h 
	$(CXX) -c $< $(CXXFLAGS)

AtlasStyle.o: AtlasStyle.cxx AtlasStyle.h 
	$(CXX) -c $< $(CXXFLAGS) 

clean:
	rm -f *~ *.o *.o~ *h.gch Analyser

# reminders:
# $@ - left target being called
# $< - first dependency
# $^ - all dependencies
# %  - any pattern
# $$ - holds the $ pattern without expanding it
