CC = g++
EXE = rpc_error rpc_object move_people_cpp
BASE = ../../..
INC =	-I$(BASE)/includes/base \
	-I$(BASE)/includes/rpc \
	-I.

#LIBDIR = -L$(BASE)
#ABSLIB = -L../../lib/base -lbase \
#         -L../../lib/appserver -lappserver \
#         -L../../lib/rpc -lrpc
#LIBS = -ljson

SRC =	$(BASE)/src/common/rpc/RpcError.cpp \
	$(BASE)/src/common/rpc/RpcObject.cpp

rpc_error:
	$(CC) -Wall -g -o rpc_error rpc_error.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

rpc_object: rpc_object.cpp
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ rpc_object.cpp $(SRC) $(INC) -o rpc_object `pkg-config --cflags --libs protobuf`

move_people_cpp: move_people.cc
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ move_people.cc addressbook.pb.cc $(SRC) $(INC) -o move_people_cpp `pkg-config --cflags --libs protobuf`

clean:
	rm -rf *.dSYM *.o $(EXE)

