SHELL = /bin/bash
CC = c++
EXE = eterm_server
BASE = ../../..

BOOST_INCLUDE = -I/opt/local/include
BOOST_LIB_DIR = -L/opt/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_filesystem-mt
BOOST_LIB = $(BOOST_LIB_DIR) $(BOOST_LIBS)

COMMON_INCLUDES = \
-I$(BASE)/includes/base \
-I$(BASE)/includes/base/concurrency \
-I$(BASE)/includes/base/container \
-I$(BASE)/includes/base/time \
-I$(BASE)/includes/base/logging \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/base/net \
-I$(BASE)/includes/netapp/common \
-I$(BASE)/includes/netapp/server/v2 \
-I$(BASE)/includes/rpc \

INCLUDES = \
$(BOOST_INCLUDE) \
$(COMMON_INCLUDES) \
-I./include \
-I./rpc_include \
-I../common/include \
-I../common/rpc_include

LIBS = \
-L$(BASE)/lib/rpc -lrpc \
-L$(BASE)/lib/netapp -lnetapp_server \
-L$(BASE)/lib/base -lbase \
$(BOOST_LIB)

GPBUFLIB = `pkg-config --cflags --libs protobuf` 

SOURCE = \
./rpc_source/*.cpp \
./source/*.cpp \
../common/rpc_source/*.cc

$(EXE):
	$(CC) -Wall -g -o $(EXE) $(EXE).cpp $(SOURCE) $(INCLUDES) -pthread $(LIBS) $(GPBUFLIB) 

userdb:
	$(CC) -Wall -g -o userdb userdb_test.cpp source/UserDb.cpp ../common/rpc_source/*.cc $(INCLUDES) -pthread $(LIBS) $(GPBUFLIB)

clean:
	rm -rf *.dSYM *.o $(EXE) userdb

