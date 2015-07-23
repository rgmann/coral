
CC = c++
# CC = clang++
CFLAGS = -Wall -Wno-deprecated-register -g -O1
BASE = ../../../..

BOOST_INCLUDE = \
-I/opt/local/include

GTEST_INCLUDE = \
-I../../third_party/gtest-1.7.0/include

INCLUDES = $(BOOST_INCLUDE) $(GTEST_INCLUDE)\
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
-I$(BASE)/includes/netapp/common \

LIBER_LIBS = \
-L$(BASE)/lib/base -lbase \
-L$(BASE)/lib/netapp -lnetapp_client \
-L$(BASE)/lib/rsync -lrsync
LIBS = -std=c++11 -stdlib=libc++ -lc++ -lpthread

BOOST_LIB_DIR = -L/opt/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_filesystem-mt -lboost_thread-mt
BOOST_LIB = $(BOOST_LIB_DIR) $(BOOST_LIBS)

# GTEST_LIB_DIR = ../../third_party/gtest-1.7.0
# GTEST_LIB = libgtest.a

# gtest_lib:
# 	$(CC) -isystem ${GTEST_LIB_DIR}/include -I${GTEST_LIB_DIR} -pthread -c ${GTEST_LIB_DIR}/src/gtest-all.cc
# 	ar -rv libgtest.a gtest-all.o

rsync_client:
	$(CC) $(CFLAGS) -o rsync_client rsync_client.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

clean:
	rm -rf *.dSYM  *.o
	rm -f rsync_client
