
CC = g++
# CC = clang++
FLAG = -Wall -c -g
LIBDIR = base
BASE = ../..

BOOST_INCLUDE = \
-I/opt/local/include

INCLUDES = $(BOOST_INCLUDE)\
-I$(BASE)/includes/$(LIBDIR) \
-I$(BASE)/includes/$(LIBDIR)/crypto \
-I$(BASE)/includes/$(LIBDIR)/file_system \
-I$(BASE)/includes/$(LIBDIR)/cli \
-I$(BASE)/includes/$(LIBDIR)/helpers \
-I$(BASE)/includes/$(LIBDIR)/container \
-I$(BASE)/includes/$(LIBDIR)/concurrency \
-I$(BASE)/includes/$(LIBDIR)/time \
-I$(BASE)/includes/$(LIBDIR)/logging \
-I$(BASE)/includes/$(LIBDIR)/net


SOURCE = \
$(BASE)/src/nix/$(LIBDIR)/*.cpp \
$(BASE)/src/nix/$(LIBDIR)/file_system/*.cpp \
$(BASE)/src/common/$(LIBDIR)/cli/*.cpp \
$(BASE)/src/common/$(LIBDIR)/helpers/*.cpp \
$(BASE)/src/nix/$(LIBDIR)/crypto/*.cpp \
$(BASE)/src/nix/$(LIBDIR)/concurrency/*.cpp \
$(BASE)/src/nix/$(LIBDIR)/time/*.cpp \
$(BASE)/src/nix/$(LIBDIR)/net/*.cpp \
$(BASE)/src/common/$(LIBDIR)/container/CircularBuffer.cpp \
$(BASE)/src/common/$(LIBDIR)/logging/*.cpp \
$(BASE)/src/common/$(LIBDIR)/file_system/*.cpp \
$(BASE)/src/common/$(LIBDIR)/net/*.cpp

all:
	$(CC) $(FLAG) $(INCLUDES) $(SOURCE) -std=c++11 -stdlib=libc++
	ar -cvq libbase.a *.o

clean:
	rm -rf  *.o *.a
