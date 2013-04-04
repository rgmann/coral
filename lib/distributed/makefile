
CC = gcc
FLAGS = -DNDEBUG
BASE = ../..
DIR = distributed
CPP = $(BASE)/src/Nix/$(DIR)/*.cpp
INC = -I$(BASE)/includes/$(DIR) 
INC += -I$(BASE)/third_party/libjson
LIBS = -lstdc++ -lpthread -L$(BASE)/third_party/libjson -ljson
LIBNAME = distr

all:
	$(CC) $(FLAGS) -Wall -c $(CPP) $(INC)
	ar -cvq lib$(LIBNAME).a *.o

clean:
	rm -rf  *.o *.a
