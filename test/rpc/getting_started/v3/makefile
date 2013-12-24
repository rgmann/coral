SHELL = /bin/bash
CC = c++
EXE = rpctest
BASE = ../../../..
INC =	-I$(BASE)/includes/base \
	-I$(BASE)/includes/base/concurrency \
	-I$(BASE)/includes/base/container \
	-I$(BASE)/includes/base/time \
	-I$(BASE)/includes/base/crypto \
	-I$(BASE)/includes/netapp/common \
	-I$(BASE)/includes/netapp/client \
	-I$(BASE)/includes/netapp/server \
	-I$(BASE)/includes/rpc \
	-I./project/server/include \
	-I./project/server/rpc_include \
	-I./project/client/include \
	-I./project/client/rpc_include \
	-I./project/common/include \
	-I./project/common/rpc_include

ABSLIB =-L$(BASE)/lib/base -lbase \
	-L$(BASE)/lib/netapp -lnetapp \
	-L$(BASE)/lib/rpc -lrpc

LIBS = `pkg-config --cflags --libs protobuf` 

#SRC =	./project/server/source/*.cpp
SOURCE =./project/server/rpc_source/*.cpp \
	./project/server/source/*.cpp \
	./project/client/rpc_source/*.cpp \
	./project/common/rpc_source/*.cc

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SOURCE) $(INC) -pthread $(LIBS) $(ABSLIB) 

clean:
	rm -rf *.dSYM *.o $(EXE)

