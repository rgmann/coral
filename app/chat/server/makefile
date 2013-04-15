BASE = ../../..
CC = g++
INC =   -I$(BASE)/includes/base \
	-I$(BASE)/includes/base/concurrency \
	-I$(BASE)/includes/base/container \
	-I$(BASE)/includes/base/cli \
        -I$(BASE)/includes/appserver
AINC = $(INC) \
       -I../common
ABSLIB = -L$(BASE)/lib/appserver -lappserver -L$(BASE)/lib/base -lbase
LIBS = -lstdc++ -lpthread
FLAGS = -Wall -g
OBJ = *.o
SRC = *.cpp ../common/*.cpp

*.o:
	$(CC) $(FLAGS) -c $(SRC) $(AINC)

server: $(OBJ)
	$(CC) $(FLAGS) -o server $(OBJ) $(AINC) $(ABSLIB) $(LIBS)

clean:
	rm -rf *.o *.dSYM server
