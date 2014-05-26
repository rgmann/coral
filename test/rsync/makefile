
CC = c++
CFLAGS = -Wall -g
BASE = ../..

BOOST_INCLUDE = \
-I/opt/local/include

INCLUDES = $(BOOST_INCLUDE)\
-I$(BASE)/includes/base \
-I$(BASE)/includes/base/cli \
-I$(BASE)/includes/base/hash \
-I$(BASE)/includes/base/crypto \
-I$(BASE)/includes/base/file_system \
-I$(BASE)/includes/base/helpers \
-I$(BASE)/includes/base/container \
-I$(BASE)/includes/base/concurrency \
-I$(BASE)/includes/base/logging \
-I$(BASE)/includes/base/time \
-I$(BASE)/includes/base/net \
-I$(BASE)/includes/rsync \
-I$(BASE)/includes/netapp/common

LIBER_LIBS = \
-L$(BASE)/lib/rsync -lrsync \
-L$(BASE)/lib/base -lbase \
-L$(BASE)/lib/netapp -lnetapp_client
LIBS = -lstdc++ -lpthread

BOOST_LIB_DIR = -L/opt/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_filesystem-mt
BOOST_LIB = $(BOOST_LIB_DIR) $(BOOST_LIBS)

segment_test:
	$(CC) $(CFLAGS) -o segment_test segment_test.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

job_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

seg_time:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

fs_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(BASE)/src/common/rsync/FileSystemInterface.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

inst_test:
	$(CC) $(CFLAGS) -o inst_test instruction_test.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

clean:
	rm -rf *.dSYM  *.o
	rm -f segment_test
	rm -f seg_time
	rm -f job_test
	rm -f fs_test
	rm -f inst_test

