CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++14
# Try testing both without and with the optimization.
# OPTFLAGS  	  = -DNDEBUG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"
INCLUDE   = -I$(BOOST_DIR)

TEST_FILES    = catch.hpp repeat.hpp

EXE 	      = main_tests
OBJ           = main_tests.o fingerprint_tests.o helpers_tests.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main_tests.cpp

fingerprint_tests.o: fingerprint_tests.cpp fingerprints_whitebox.hpp ../fingerprints.hpp ../fingerprints.cpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c fingerprint_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c helpers_tests.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
