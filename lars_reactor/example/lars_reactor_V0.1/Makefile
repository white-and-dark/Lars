CXX=g++
CFLAGS=-g -O2 -Wall -fPIC -Wno-deprecated 

INC=-I../../include
LIB=-L../../lib -lreactor 
OBJS = $(addsuffix .o, $(basename $(wildcard *.cc)))

all:
	# $(CXX) -o hello_lars $(CFLAGS)  hello_lars.cpp $(INC) $(LIB)
	$(CXX) -o lars_reactor_test $(CFLAGS)  lars_reactor.cpp $(INC) $(LIB)

clean:
	-rm -f *.o hello_lars lars_reactor_test
