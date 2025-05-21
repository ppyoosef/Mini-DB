CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
SRC = src/DBMS.cpp src/Parser.cpp src/Table.cpp main.cpp
OUT = mini-dbms

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
