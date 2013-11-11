CC = g++
EXE = server
ROOT = ../../../..
BASE = $(ROOT)/third_party/libjson
INC = -I$(BASE) \
      -I$(ROOT)/includes/base \
      -I$(ROOT)/includes/base/concurrency \
      -I$(ROOT)/includes/base/container \
      -I$(ROOT)/includes/base/time \
      -I$(ROOT)/includes/appserver \
      -I$(ROOT)/includes/rpc \
		-I./include \
		-I./rpc_include \
		-I../common/rpc_include

LIBDIR = -L$(BASE)
ABSLIB = -L$(ROOT)/lib/base -lbase \
         -L$(ROOT)/lib/appserver -lappserver \
         -L$(ROOT)/lib/rpc -lrpc
LIBS = -ljson

SRC = ./source/*.cpp \
		./rpc_source/*.cpp \
		../common/rpc_source/*.cpp

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

clean:
	rm -rf *.dSYM *.o $(EXE)

