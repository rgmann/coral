
CC = gcc
INC =	-I../../includes/base \
	-I../../includes/base/cli \
	-I../../includes/base/hash \
	-I../../includes/base/crypto \
	-I../../includes/base/file_system \
	-I../../includes/base/helpers \
	-I../../includes/base/container \
	-I../../includes/base/concurrency \
	-I../../includes/rsync
ABSLIB = -L../../lib/base -lbase -L../../lib/rsync -lrsync
LIBS = -lstdc++ -lpthread

rsync:
	$(CC) -Wall -g -o rsync rsynctest.cpp $(INC) $(ABSLIB) $(LIBS)

segm:
	$(CC) -Wall -g -o segm segmentertest.cpp $(INC) $(ABSLIB) $(LIBS)

flow:
	$(CC) -Wall -g -o flow flowtest.cpp $(INC) $(ABSLIB) $(LIBS)

clean:
	rm -rf  *.o rsync segm flow *.dSYM
