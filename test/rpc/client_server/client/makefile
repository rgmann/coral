ROOT = ../../../..
BASE = $(ROOT)/third_party/libjson
CC = gcc
EXE = client
INC =   -I$(ROOT)/includes/base \
	-I$(ROOT)/includes/base/concurrency \
	-I$(ROOT)/includes/base/container \
	-I$(ROOT)/includes/base/cli \
        -I$(ROOT)/includes/appserver \
        -I$(ROOT)/includes/rpc \
        -I./include \
	-I./rpc_include \
	-I../common/include \
	-I../common/rpc_include

JLIB = -L$(BASE) -ljson
ABSLIB = -L$(ROOT)/lib/base -lbase \
	-L$(ROOT)/lib/appserver -lappserver \
	-L$(ROOT)/lib/rpc -lrpc 
LIBS = $(JLIB) -lstdc++ -lpthread
FLAGS = -Wall -g
# -mmacosx-version-min=10.6
#SRC = 	./source/*.cpp
SRC = 	./rpc_source/*.cpp \
	../common/rpc_source/*.cpp

$(EXE):
	$(CC) $(FLAGS) -o $(EXE) $(EXE).cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

clean:
	rm -rf *.o *.dSYM client
