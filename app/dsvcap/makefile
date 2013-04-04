
CC = gcc
INC = -I../../includes/base \
      -I../../includes/base/file_system \
		-I../../includes/base/cli
ABSLIB = -L../../lib/base -lbase
LIBS = -lstdc++ -lpthread
	
dsvcap:
	$(CC) -Wall -g -o dsvcap dsvcapture.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o dsvcap *.dSYM
