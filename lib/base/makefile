
CC = g++
FLAG = -Wall -c -g
LIBDIR = base
<<<<<<< HEAD
CPP = ../../src/nix/$(LIBDIR)/*.cpp \
		../../src/nix/$(LIBDIR)/file_system/*.cpp \
		../../src/common/$(LIBDIR)/cli/*.cpp \
		../../src/common/$(LIBDIR)/helpers/*.cpp \
		../../src/nix/$(LIBDIR)/crypto/*.cpp \
		../../src/nix/$(LIBDIR)/concurrency/*.cpp \
		../../src/common/$(LIBDIR)/container/*.cpp \
		../../src/common/$(LIBDIR)/file_system/*.cpp
INC = -I../../includes/$(LIBDIR) \
		-I../../includes/$(LIBDIR)/crypto \
		-I../../includes/$(LIBDIR)/file_system \
		-I../../includes/$(LIBDIR)/cli \
		-I../../includes/$(LIBDIR)/helpers \
		-I../../includes/$(LIBDIR)/container \
		-I../../includes/$(LIBDIR)/concurrency
=======
CPP =	../../src/nix/$(LIBDIR)/*.cpp \
	../../src/nix/$(LIBDIR)/file_system/*.cpp \
	../../src/common/$(LIBDIR)/cli/*.cpp \
	../../src/common/$(LIBDIR)/helpers/*.cpp \
	../../src/nix/$(LIBDIR)/crypto/*.cpp \
	../../src/nix/$(LIBDIR)/concurrency/*.cpp \
	../../src/nix/$(LIBDIR)/time/*.cpp \
	../../src/common/$(LIBDIR)/concurrency/*.cpp \
	../../src/common/$(LIBDIR)/container/*.cpp \
	../../src/common/$(LIBDIR)/logging/*.cpp
INC =	-I../../includes/$(LIBDIR) \
	-I../../includes/$(LIBDIR)/crypto \
	-I../../includes/$(LIBDIR)/file_system \
	-I../../includes/$(LIBDIR)/cli \
	-I../../includes/$(LIBDIR)/helpers \
	-I../../includes/$(LIBDIR)/container \
	-I../../includes/$(LIBDIR)/concurrency \
	-I../../includes/$(LIBDIR)/time \
	-I../../includes/$(LIBDIR)/logging \
>>>>>>> 01387b9170a26f65ee6fc904cc70e465a313b7e1

all:
	$(CC) $(FLAG) $(CPP) $(INC)
	ar -cvq libbase.a *.o

clean:
	rm -rf  *.o *.a
