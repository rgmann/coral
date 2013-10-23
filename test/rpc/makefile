CC = g++
EXE = rpctest
BASE = ../../third_party/libjson
INC = -I$(BASE) \
      -I../../includes/base \
      -I../../includes/base/concurrency \
      -I../../includes/base/container \
      -I../../includes/base/time \
      -I../../includes/appserver \
      -I../../includes/rpc \
		-I./includes

LIBDIR = -L$(BASE)
ABSLIB = -L../../lib/base -lbase \
         -L../../lib/appserver -lappserver \
         -L../../lib/rpc -lrpc
LIBS = -ljson

SRC = ./src/*.cpp

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

clean:
	rm -rf *.dSYM *.o $(EXE)

