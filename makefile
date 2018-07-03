CC        = g++
CCFLAGS   = -Wall -pedantic -std=c++14
# OPTFLAGS  = -DNDEBUG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"

INCLUDE   = -I$(BOOST_DIR)
LDFLAGS   = -L$(BOOST_DIR) -static
LDLIBS    = -lboost_program_options

EXE       = fingerprints
OBJ       = main.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

main.o: main.cpp fingerprints.cpp fingerprints.hpp helpers.hpp params.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main.cpp

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
