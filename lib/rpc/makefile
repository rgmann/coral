SHELL = /bin/bash
CC = g++
FLAG = -Wall -c -g
LIBNAME = rpc
BASE = ../..

SOURCE = \
$(BASE)/src/common/$(LIBNAME)/*.cpp

INCLUDES = \
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
