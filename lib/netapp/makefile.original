SHELL = /bin/bash
CC = g++
CFLAGS = -Wall -c -g
LIBNAME = netapp
BASE = ../..

BOOST_INCLUDE = \
-I/opt/local/include

COMMON_SOURCE = \
$(BASE)/src/common/$(LIBNAME)/common/*.cpp \

COMMON_INCLUDES = \
$(BOOST_INCLUDE) \
-I$(BASE)/includes/base \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/base/file_system \
-I$(BASE)/includes/base/cli \
-I$(BASE)/includes/base/net \
-I$(BASE)/includes/base/helpers \
-I$(BASE)/includes/base/time \
-I$(BASE)/includes/base/logging \
-I$(BASE)/includes/base/container \
-I$(BASE)/includes/base/concurrency \
-I$(BASE)/includes/netapp/common \

SERVER_INCLUDES = $(COMMON_INCLUDES) \
	-I$(BASE)/includes/netapp/server

SERVER_SOURCE = $(COMMON_SOURCE) \
	$(BASE)/src/common/$(LIBNAME)/server/*.cpp

CLIENT_INCLUDES = $(COMMON_INCLUDES) \
	-I$(BASE)/includes/netapp/client

CLIENT_SOURCE = $(COMMON_SOURCE) \
	$(BASE)/src/common/$(LIBNAME)/client/*.cpp


netapp_common:
	$(CC) $(CFLAGS) $(COMMON_SOURCE) $(COMMON_INCLUDES)
	ar -cvq lib$@.a *.o

netapp_client:
	$(CC) $(CFLAGS) $(CLIENT_SOURCE) $(CLIENT_INCLUDES)
	ar -cvq lib$@.a *.o

netapp_server:
	$(CC) $(CFLAGS) $(SERVER_SOURCE) $(SERVER_INCLUDES)
	ar -cvq lib$@.a *.o 

# For test applications where everything happens locally.
netapp_all:
	$(CC) $(CFLAGS) $(SERVER_SOURCE) $(SERVER_INCLUDES) $(CLIENT_SOURCE) $(CLIENT_INCLUDES)
	ar -cvq lib$@.a *.o

all: $(LIBNAME)_client $(LIBNAME)_server $(LIBNAME)_all

clean:
	rm -rf  *.o *.a
