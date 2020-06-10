

CGLIB := libCaloGraphy.so
CGG4LIB := libCGG4.so


CXX := g++
DEPFLAGS := -MD -MP
OPT := -g -O0

CXXFLAGS := $(OPT) $(DEPFLAGS) -fPIC -I../calography/
LDFLAGS := -fPIC -shared

G4CXXFLAGS := -I$(G4INCLUDE)
G4LIBS := -L$(G4LIB)/$(G4SYSTEM) -lG4global

CGSRC :=  node.cc process.cc track.cc
G4SRC := CGG4Interface.cc

CGOBJS := $(CGSRC:.cc=.o)
G4OBJS := $(G4SRC:.cc=.o)

.PHONY: clean all

all: $(CGLIB) $(CGG4LIB)


$(CGOBJS): %.o: %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

# build the calography base package
$(CGLIB):  $(CGOBJS)
	$(CXX) $(LDFLAGS) -o$@  $^


$(G4OBJS): %.o: %.cc
	$(CXX) -c $(CXXFLAGS) $(G4CXXFLAGS) -o $@ $^

# build the Geant4 interface
$(CGG4LIB): $(CGLIB) $(G4OBJS)
	$(CXX) $(LDFLAGS) $(G4LIBS) -o $@ $^ -L./ -lCaloGraphy


clean:
	rm -f *.so *.o *.d
