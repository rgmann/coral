CC = g++
#BASE = ../../../../third_party/mongo-cxx-driver-nightly
BASE = ../../../../third_party/libjson
INC = -I$(BASE) \
      -I../include \
      -I./rpc_test \
      -I../../../../includes/base \
      -I../../../../includes/base/concurrency \
      -I../../../../includes/base/container \
      -I../../../../includes/base/time \
      -I../../../../includes/appserver
#INC =   -I$(BASE)/src \
	-I$(BASE)/src/mongo \
	-I$(BASE)/src/mongo/client \
	-I../models/includes \
	-I../../../../includes/base \
	-I../../../../includes/base/file_system \
	-I../../../../includes/base/time
LIBDIR = -L$(BASE)
ABSLIB = -L../../../../lib/base -lbase \
         -L../../../../lib/appserver -lappserver
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

SRC = ../src/JsonObject.cpp \
      ../src/JsonElement.cpp
#SSRC = ../src/Structure.cpp 
SSRC = ../src/JsonTransportObject.cpp \
       ../src/Structure.cpp

RPCSRC = ../src/Structure.cpp \
      ../src/JsonTransportObject.cpp \
      rpc_test/RpcError.cpp \
      rpc_test/RpcObject.cpp \
      rpc_test/RpcPacket.cpp \
      rpc_test/RpcReturnValue.cpp \
      rpc_test/RpcMarshalledCall.cpp \
      rpc_test/RpcClient.cpp \
      rpc_test/RpcClientResource.cpp \
      rpc_test/RpcServerResource.cpp \
      rpc_test/RpcServer.cpp \
      rpc_test/Calculator.cpp \
      rpc_test/CalculatorClientStub.cpp

crud:
	$(CC) -Wall -g -o crudtest crudtest.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

tplate:
	$(CC) -Wall -g -o tplate ttest.cpp $(SSRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

rpc:
	$(CC) -Wall -g -o rpctest rpctest.cpp $(RPCSRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

excp:
	$(CC) -Wall -g -o excptest excptest.cpp

svfs:
	$(CC) -Wall -g -o svfstest svfstest.cpp

clean:
	rm -rf *.dSYM *.o excptest crudtest tplate rpctest svfstest

