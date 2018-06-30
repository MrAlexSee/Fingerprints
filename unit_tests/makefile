CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++14

# Try testing both without and with the optimization.
# OPTFLAGS  	  = -DNDEBUG -O3

TEST_FILES    = catch.hpp repeat.hpp

EXE 	      = main_tests
OBJ           = main_tests.o fingerprint_tests.o helpers_tests.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c main_tests.cpp

fingerprint_tests.o: fingerprint_tests.cpp fingerprints_whitebox.hpp ../fingerprints.hpp ../fingerprints.cpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c fingerprint_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c helpers_tests.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
