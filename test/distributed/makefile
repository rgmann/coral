
CC = gcc
FLAGS = -DNDEBUG -Wall -g
INC =  -I../../includes/base \
       -I../../includes/distributed \
       -I../../lib/libjson 
ABSLIB = -L../../lib/base -lbase 
ABSLIB += -L../../lib/distributed -ldistr 
ABSLIB += -ljson
LIBS = -lstdc++ -lpthread

vectorclock:
	$(CC) $(FLAGS) -o vectorclock vectortest.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o vectorclock *.dSYM
