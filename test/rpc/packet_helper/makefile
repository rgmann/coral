SHELL = /bin/bash
CC = c++
EXE = packettest
BASE = ../../..
INC =	-I$(BASE)/includes/base \
	-I$(BASE)/includes/base/concurrency \
	-I$(BASE)/includes/base/container \
	-I$(BASE)/includes/base/time \
	-I$(BASE)/includes/appserver \
	-I$(BASE)/includes/rpc \
	-I./project/server/include \
	-I./project/server/rpc_include \
	-I./project/client/include \
	-I./project/client/rpc_include \
	-I./project/common/include \
	-I./project/common/rpc_include

ABSLIB = -L$(BASE)/lib/base -lbase \
	-L$(BASE)/lib/appserver -lappserver \
	-L$(BASE)/lib/rpc -lrpc

LIBS = `pkg-config --cflags --libs protobuf` 

SOURCE =

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SOURCE) $(INC) -pthread $(LIBS) $(ABSLIB) 

clean:
	rm -rf *.dSYM *.o $(EXE)

