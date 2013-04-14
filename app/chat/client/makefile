BASE = ../../..
CC = gcc
INC =   -I$(BASE)/includes/base \
	-I$(BASE)/includes/base/cli \
        -I$(BASE)/includes/appserver
AINC = $(INC) \
       -I../common
ABSLIB = -L$(BASE)/lib/base -lbase -L$(BASE)/lib/appserver -lappserver
LIBS = -lstdc++ -lpthread
FLAGS = -Wall -g
OBJ = *.o
SRC = *.cpp ../common/*.cpp

*.o:
	$(CC) $(FLAGS) -c $(SRC) $(AINC)

client: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o client $(AINC) $(ABSLIB) $(LIBS)

clean:
	rm -rf *.o *.dSYM client
