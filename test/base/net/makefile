
CC = gcc
BASE = ../../..
INCLUDES =-I$(BASE)/includes/base \
	-I$(BASE)/includes/base/cli \
	-I$(BASE)/includes/base/hash \
	-I$(BASE)/includes/base/net \
	-I$(BASE)/includes/base/crypto \
	-I$(BASE)/includes/base/file_system \
	-I$(BASE)/includes/base/helpers \
	-I$(BASE)/includes/base/container \
	-I$(BASE)/includes/base/concurrency \
	-I$(BASE)/includes/base/logging \
	-I$(BASE)/includes/base/time
ABSLIB = -L$(BASE)/lib/base -lbase
LIBS = -lstdc++ -lpthread
	
client:
	$(CC) -Wall -g -o client testclient.cpp $(INCLUDES) $(ABSLIB) $(LIBS)

server:
	$(CC) -Wall -g -o server testserver.cpp $(INCLUDES) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o client server *.dSYM
