SHELL = /bin/bash
CC = g++
FLAG = -Wall -c -g
LIBNAME = rpc
BASE = ../..

BOOST_INCLUDE = \
-I/opt/local/include \

SOURCE = \
$(BASE)/src/common/$(LIBNAME)/RpcClient.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcClientResource.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcException.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcMarshalledCall.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcObject.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcPacket.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcServer.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcServerResource.cpp \
$(BASE)/src/common/$(LIBNAME)/RpcSupervisor.cpp \
$(BASE)/src/common/$(LIBNAME)/BlockingRpcSupervisor.cpp \
$(BASE)/src/common/$(LIBNAME)/AsyncRpcSupervisor.cpp \

INCLUDES = $(BOOST_INCLUDE) \
-I$(BASE)/includes/base \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/base/net \
-I$(BASE)/includes/base/file_system \
-I$(BASE)/includes/base/cli \
-I$(BASE)/includes/base/helpers \
-I$(BASE)/includes/base/time \
-I$(BASE)/includes/base/logging \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/base/container \
-I$(BASE)/includes/base/concurrency \
-I$(BASE)/includes/netapp/common \
-I$(BASE)/includes/netapp/client \
-I$(BASE)/includes/netapp/server \
-I$(BASE)/includes/$(LIBNAME)

all:
	$(CC) $(FLAG) $(SOURCE) $(INCLUDES) `pkg-config --cflags protobuf`
	ar -cvq lib$(LIBNAME).a *.o
#`pkg-config --cflags --libs protobuf`

clean:
	rm -rf  *.o *.a
