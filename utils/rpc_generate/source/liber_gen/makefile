SHELL = /bin/bash
CC = c++
BIN = protoc-gen-libergen
BASE=../../../..

GPBUFLIB = `pkg-config --cflags --libs protobuf` -lprotoc

INCLUDE=\
-I$(BASE)/includes/base/helpers

SOURCE=\
LiberFileGenerator.cpp

LIBS=\
-L$(BASE)/lib/base -lbase \
-lstdc++ \
$(GPBUFLIB) \

libergen:
	$(CC) -Wall -g -o $(BIN) libergen.cpp $(SOURCE) $(INCLUDE) -pthread $(LIBS) 

clean:
	rm -rf *.dSYM *.o $(BIN)

