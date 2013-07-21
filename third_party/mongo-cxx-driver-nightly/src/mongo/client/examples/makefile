CC = g++
BASE = /home/rvaughan/Development/install_pkgs/mongo-cxx-driver-nightly
INC =   -I$(BASE)/src \
	-I$(BASE)/src/mongo \
	-I$(BASE)/src/mongo/client \
	-I$(BASE)/src/mongo/util
LIBDIR = -L$(BASE)
LIBS =  -lmongoclient \
	-lboost_thread-mt \
	-lboost_filesystem \
	-lboost_program_options \
	-lboost_system

test:
	$(CC) -Wall -g -o test test.cpp $(INC) -pthread $(LIBDIR) $(LIBS)

clean:
	rm -rf *.o test

