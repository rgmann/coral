CC = g++
#BASE = ../../../../third_party/mongo-cxx-driver-nightly
BASE = ../../../../third_party/libjson
INC = -I$(BASE) \
      -I../../include \
      -I../../../../includes/base \
      -I../../../../includes/base/concurrency \
      -I../../../../includes/base/container \
      -I../../../../includes/appserver
#INC =   -I$(BASE)/src \
	-I$(BASE)/src/mongo \
	-I$(BASE)/src/mongo/client \
	-I../models/includes \
	-I../../../../includes/base \
	-I../../../../includes/base/file_system \
	-I../../../../includes/base/time
LIBDIR = -L$(BASE)
#ABSLIB = -L../../lib/base -lbase
LIBS = -ljson
#LIBS =  -lmongoclient \
	-lboost_thread-mt \
	-lboost_filesystem \
	-lboost_program_options \
	-lboost_system
#SRC = 	../models/src/Collection.cpp \
	../models/src/MongoConnection.cpp \
	../models/src/GenericModel.cpp \
	../models/src/Person.cpp \
        ../models/src/PersonCollection.cpp

SRC = ../../src/Structure.cpp \
      ../../src/TransportObject.cpp \
      RpcPacket.cpp \
      RpcReturnValue.cpp \
      RpcMarshalledCall.cpp \
      RpcClient.cpp \
      RpcClientResource.cpp \
      RpcServer.cpp \
      RpcServerResource.cpp \
      Calculator.cpp \
      CalculatorServerStub.cpp \
      CalculatorClientStub.cpp 

rpc:
	$(CC) -Wall -g -o rpctest rpctest.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

clean:
	rm -rf *.dSYM *.o rpctest

