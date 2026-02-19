ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --glibs) -lMathMore -lRooFit -lRooStats -lRooFitCore -lFoam -lMinuit -lTreePlayer -lHistFactory -lXMLIO

CXX           = g++
CXXFLAGS      = -O0 -Wall -fPIC -g $(ROOTCFLAGS) 
CXXLIBS       = $(ROOTLIBS)

default : CreateNNInput

CreateNNInput: CreateNNInput.o TreeReader.o
	$(CXX) -o $@ $^ $(CXXLIBS)

CreateNNInput.o: CreateNNInput.cxx
	$(CXX) -c $< $(CXXFLAGS)

TreeReader.o: TreeReader.cxx TreeReader.h 
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm -f *~ *.o *.o~ *h.gch CreateNNInput

# reminders:
# $@ - left target being called
# $< - first dependency
# $^ - all dependencies
# %  - any pattern
# $$ - holds the $ pattern without expanding it
