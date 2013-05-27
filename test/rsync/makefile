
CC = gcc
INC =	-I../../includes/base \
	-I../../includes/base/cli \
	-I../../includes/base/hash \
	-I../../includes/base/crypto \
	-I../../includes/base/file_system \
	-I../../includes/base/helpers \
	-I../../includes/base/container \
	-I../../includes/base/concurrency \
	-I../../includes/appserver \
	-I../../includes/rsync \
       -I../../includes/rsync/packets

ABSLIB = -L../../lib/base -lbase \
         -L../../lib/rsync -lrsync \
         -L../../lib/appserver -lappserver
LIBS = -lstdc++ -lpthread

rsync:
	$(CC) -Wall -g -o rsync rsynctest.cpp $(INC) $(ABSLIB) $(LIBS)

packet:
	$(CC) -Wall -g -o packet packettest.cpp $(INC) $(ABSLIB) $(LIBS)

flow:
	$(CC) -Wall -g -o flow flowtest.cpp $(INC) $(ABSLIB) $(LIBS)

class:
	$(CC) -Wall -g -o class classtest.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o rsync packet flow class *.dSYM
