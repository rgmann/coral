
CC = g++
FLAG = -Wall -c -g
LIBNAME = appserver
CPP = ../../src/common/$(LIBNAME)/*.cpp
INC = -I../../includes/base \
      -I../../includes/base/crypto \
      -I../../includes/base/file_system \
      -I../../includes/base/cli \
      -I../../includes/base/helpers \
      -I../../includes/base/container \
      -I../../includes/base/concurrency \
      -I../../includes/appserver

all:
	$(CC) $(FLAG) $(CPP) $(INC)
	ar -cvq lib$(LIBNAME).a *.o

clean:
	rm -rf  *.o *.a
