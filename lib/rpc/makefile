SHELL = /bin/bash
CC = g++
FLAG = -Wall -c -g
LIBNAME = rpc
SOURCE = ../../src/common/$(LIBNAME)/*.cpp
INCLUDES = \
	-I../../includes/base \
	-I../../includes/base/crypto \
	-I../../includes/base/net \
	-I../../includes/base/file_system \
	-I../../includes/base/cli \
	-I../../includes/base/helpers \
	-I../../includes/base/time \
	-I../../includes/base/crypto \
	-I../../includes/base/container \
	-I../../includes/base/concurrency \
	-I../../includes/netapp/common \
	-I../../includes/netapp/client \
	-I../../includes/netapp/server \
	-I../../includes/$(LIBNAME)

all:
	$(CC) $(FLAG) $(SOURCE) $(INCLUDES) `pkg-config --cflags protobuf`
	ar -cvq lib$(LIBNAME).a *.o
#`pkg-config --cflags --libs protobuf`

clean:
	rm -rf  *.o *.a
