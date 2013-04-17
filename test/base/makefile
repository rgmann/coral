
CC = gcc
INC =	-I../../includes/base \
	-I../../includes/base/cli \
	-I../../includes/base/hash \
	-I../../includes/base/crypto \
	-I../../includes/base/file_system \
	-I../../includes/base/helpers \
	-I../../includes/base/container \
	-I../../includes/base/concurrency
ABSLIB = -L../../lib/base -lbase
LIBS = -lstdc++ -lpthread
	
client:
	$(CC) -Wall -g -o client testclient.cpp $(INC) $(ABSLIB) $(LIBS)

server:
	$(CC) -Wall -g -o server testserver.cpp $(INC) $(ABSLIB) $(LIBS)

thread:
	$(CC) -Wall -g -o thread threadtest.cpp $(INC) $(ABSLIB) $(LIBS)

sem:
	$(CC) -Wall -g -o sem semtest.cpp $(INC) $(ABSLIB) $(LIBS)
	
fs:
	$(CC) -Wall -g -o fs fstest.cpp $(INC) $(ABSLIB) $(LIBS)

mon:
	$(CC) -Wall -g -o mon montest.cpp $(INC) $(ABSLIB) $(LIBS)

dsv:
	$(CC) -Wall -g -o dsv dsvtest.cpp $(INC) $(ABSLIB) $(LIBS)

rsync:
	$(CC) -Wall -g -o rsync rsynctest.cpp $(INC) $(ABSLIB) $(LIBS)
	
arg:
	$(CC) -Wall -g -o arg argtest.cpp $(INC) $(ABSLIB) $(LIBS)

hashtab:
	$(CC) -Wall -g -o hashtab hashtest.cpp $(INC) $(ABSLIB) $(LIBS)

segm:
	$(CC) -Wall -g -o segm segmentertest.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o client server thread sem fs mon dsv rsync arg hashtab segm *.dSYM
