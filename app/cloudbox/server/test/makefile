CC = g++
BASE = ../../../../third_party/mongo-cxx-driver-nightly
INC =   -I$(BASE)/src \
	-I$(BASE)/src/mongo \
	-I$(BASE)/src/mongo/client \
	-I../models/includes \
	-I../../../../includes/base \
	-I../../../../includes/base/file_system
LIBDIR = -L$(BASE)
ABSLIB = -L../../lib/base -lbase
LIBS =  -lmongoclient \
	-lboost_thread-mt \
	-lboost_filesystem \
	-lboost_program_options \
	-lboost_system
SRC = 	../models/src/Collection.cpp \
	../models/src/MongoConnection.cpp \
	../models/src/GenericModel.cpp \
	../models/src/Person.cpp \
        ../models/src/PersonCollection.cpp

person:
	$(CC) -Wall -g -o person persontest.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS)

tutorial:
	$(CC) -Wall -g -o tutorial tutorial.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS)

ugtest:
	$(CC) -Wall -g -o ugtest ugtest.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS)

file:
	$(CC) -Wall -g -o filetest filetest.cpp $(SRC) $(INC) -pthread $(LIBDIR) $(LIBS) $(ABSLIB)

clean:
	rm -rf *.o person tutorial ugtest filetest

