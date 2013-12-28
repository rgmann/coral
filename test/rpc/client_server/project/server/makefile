SHELL = /bin/bash
CC = c++
EXE = rpc_server
BASE = ../../../../..
INC =	-I$(BASE)/includes/base \
	-I$(BASE)/includes/base/concurrency \
	-I$(BASE)/includes/base/container \
	-I$(BASE)/includes/base/time \
	-I$(BASE)/includes/base/crypto \
	-I$(BASE)/includes/base/net \
	-I$(BASE)/includes/netapp/common \
	-I$(BASE)/includes/netapp/client \
	-I$(BASE)/includes/netapp/server \
	-I$(BASE)/includes/rpc \
	-I./include \
	-I./rpc_include \
	-I../common/include \
	-I../common/rpc_include

ABSLIB =-L$(BASE)/lib/base -lbase \
	-L$(BASE)/lib/netapp -lnetapp \
	-L$(BASE)/lib/rpc -lrpc

LIBS = `pkg-config --cflags --libs protobuf` 

SOURCE =./rpc_source/*.cpp \
	./source/*.cpp \
	../common/rpc_source/*.cc

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SOURCE) $(INC) -pthread $(LIBS) $(ABSLIB) 

clean:
	rm -rf *.dSYM *.o $(EXE)

