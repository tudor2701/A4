CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -fno-inline -fPIC -fPIE

# Vorkompilierte Object-Files der Praktikumsumgebung.
# Default: Linux (Lernraum-Testat). Lokal (Apple Silicon) uebersteuern mit z.B.
#   make TEST_MV_O=test_mv_macM1.o UNIT_O=unit_macM1.o
TEST_MV_O ?= test_mv.o
UNIT_O    ?= unit.o

.PHONY: all clean

all: loesung test_mv

loesung: sparse_matrix.o loeser.o $(UNIT_O)
	$(CXX) $(CXXFLAGS) -o $@ sparse_matrix.o loeser.o $(UNIT_O)

loeser.o: loeser.cpp sparse_matrix.h vector.h unit.h
	$(CXX) $(CXXFLAGS) -c $<

test_mv: sparse_matrix.o $(TEST_MV_O)
	$(CXX) $(CXXFLAGS) -o $@ sparse_matrix.o $(TEST_MV_O)

test_sparse: sparse_matrix.o test_sparse.o
	$(CXX) $(CXXFLAGS) -o $@ sparse_matrix.o test_sparse.o

test_sparse.o: test_sparse.cpp sparse_matrix.h vector.h mapra_test.h
	$(CXX) $(CXXFLAGS) -c $<

sparse_matrix.o: sparse_matrix.cpp sparse_matrix.h vector.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) sparse_matrix.o loeser.o test_sparse.o loesung test_mv test_sparse
