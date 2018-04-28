INC_SVR_COMM = -I.  -I/usr/local/include
LIB_SVR_COMM = -L/usr/local/lib 

INC_ALL=$(INC_SVR_COMM) 
LIB_ALL=$(LIB_SVR_COMM)

BINARY = Lru_test_bin

all:$(BINARY)


.SUFFIXES: .o .cpp
CXX = g++
CC = gcc

CXXFLAGS= -g -Wall -ggdb3 -DDEBUG_TEST -D_ReDirect

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(INC_ALL)

Lru_test_bin:lru_test_main.o  lru_hash.o
	$(CXX) $(CXXFLAGS)  -o $@ $^ $(LIB_ALL)
clean:
	rm -f *.o *~ $(BINARY)

strip:
	strip $(BINARY)
