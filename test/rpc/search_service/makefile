SHELL = /bin/bash
CC = c++
EXE = search
BASE = ../../..

BOOST_INCLUDE = -I/opt/local/include
BOOST_LIB_DIR = -L/opt/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_filesystem-mt
BOOST_LIB = $(BOOST_LIB_DIR) $(BOOST_LIBS)

INCLUDE =$(BOOST_INCLUDE)\
-I$(BASE)/includes/base \
-I$(BASE)/includes/base/logging \
-I$(BASE)/includes/base/concurrency \
-I$(BASE)/includes/base/container \
-I$(BASE)/includes/base/time \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/netapp/common \
-I$(BASE)/includes/netapp/client \
-I$(BASE)/includes/netapp/server \
-I$(BASE)/includes/rpc \
-I./server/include \
-I./server/rpc_include \
-I./client/include \
-I./client/rpc_include \
-I./common/include \
-I./common/rpc_include

LIBS =\
-L$(BASE)/lib/base -lbase \
-L$(BASE)/lib/netapp -lnetapp_all \
-L$(BASE)/lib/rpc -lrpc \
$(BOOST_LIB)

GPBUFLIB = `pkg-config --cflags --libs protobuf` 
LIBS += $(GPBUFLIB)

#SRC =	./project/server/source/*.cpp
SOURCE =\
./server/rpc_source/*.cpp \
./client/rpc_source/*.cpp \
./common/rpc_source/*.cc

$(EXE):
	$(CC) -Wall -g -o $(EXE) search_service.cpp $(SOURCE) $(INCLUDE) -pthread $(LIBS) 

clean:
	rm -rf *.dSYM *.o $(EXE)

