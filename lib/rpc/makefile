
CC = g++
FLAG = -Wall -c -g
LIBNAME = rpc
JSON = ../../third_party/libjson
LIBS = -L$(JSON) -ljson
CPP = ../../src/common/$(LIBNAME)/*.cpp
INC = -I$(JSON) \
		-I../../includes/base \
      -I../../includes/base/crypto \
      -I../../includes/base/file_system \
      -I../../includes/base/cli \
      -I../../includes/base/helpers \
      -I../../includes/base/time \
      -I../../includes/base/container \
      -I../../includes/base/concurrency \
      -I../../includes/appserver \
      -I../../includes/$(LIBNAME)

all:
	$(CC) $(FLAG) $(CPP) $(INC)
	ar -cvq lib$(LIBNAME).a *.o

clean:
	rm -rf  *.o *.a
