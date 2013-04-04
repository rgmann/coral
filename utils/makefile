
CC = gcc
INC =	-I../includes/base \
	-I../includes/base/cli \
	-I../../includes/base/hash 
ABSLIB = -L../lib/base -lbase
LIBS = -lstdc++ -lpthread

compare:
	$(CC) -Wall -g -o compare bytecompare.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o compare *.dSYM
