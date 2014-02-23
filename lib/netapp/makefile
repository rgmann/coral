SHELL = /bin/bash
CC = g++
CFLAGS = -Wall -c -g
LIBNAME = netapp
BASE = ../..

COMMON_SOURCE = \
$(BASE)/src/common/$(LIBNAME)/common/*.cpp \

COMMON_INCLUDES = \
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
	-I$(BASE)/includes/netapp/server/v2

SERVER_SOURCE = $(COMMON_SOURCE) \
	$(BASE)/src/common/$(LIBNAME)/server/v2/*.cpp

CLIENT_INCLUDES = $(COMMON_INCLUDES) \
	-I$(BASE)/includes/netapp/client

CLIENT_SOURCE = $(COMMON_SOURCE) \
	$(BASE)/src/common/$(LIBNAME)/client/*.cpp

netapp_client:
	$(CC) $(CFLAGS) $(CLIENT_SOURCE) $(CLIENT_INCLUDES)
	ar -cvq lib$(LIBNAME)_client.a *.o

netapp_server:
	$(CC) $(CFLAGS) $(SERVER_SOURCE) $(SERVER_INCLUDES)
	ar -cvq lib$(LIBNAME)_server.a *.o 

all: $(LIBNAME)_client $(LIBNAME)_server

clean:
	rm -rf  *.o *.a
