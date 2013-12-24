SHELL = /bin/bash
CC = g++
FLAG = -Wall -c -g
LIBNAME = netapp
SOURCE = ../../src/common/$(LIBNAME)/common/*.cpp \
	../../src/common/$(LIBNAME)/server/*.cpp \
	../../src/common/$(LIBNAME)/client/*.cpp
INCLUDES =-I../../includes/base \
	-I../../includes/base/crypto \
	-I../../includes/base/file_system \
	-I../../includes/base/cli \
	-I../../includes/base/net \
	-I../../includes/base/helpers \
	-I../../includes/base/time \
	-I../../includes/base/container \
	-I../../includes/base/concurrency \
	-I../../includes/netapp/common \
	-I../../includes/netapp/server \
	-I../../includes/netapp/client

all:
	$(CC) $(FLAG) $(SOURCE) $(INCLUDES)
	ar -cvq lib$(LIBNAME).a *.o

clean:
	rm -rf  *.o *.a
