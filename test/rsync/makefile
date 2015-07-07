
CC = c++
# CC = clang++
CFLAGS = -Wall -Wno-deprecated-register -g -O1
BASE = ../..

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

GTEST_LIB_DIR = ../../third_party/gtest-1.7.0
GTEST_LIB = libgtest.a

gtest_lib:
	$(CC) -isystem ${GTEST_LIB_DIR}/include -I${GTEST_LIB_DIR} -pthread -c ${GTEST_LIB_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

segment_test:
	$(CC) $(CFLAGS) -o segment_test segment_test.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

job_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

node_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

end_to_end:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

rsync_test_suite: gtest_lib
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB) $(GTEST_LIB)

seg_time:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

fs_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(BASE)/src/common/rsync/FileSystemInterface.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

inst_test:
	$(CC) $(CFLAGS) -o inst_test instruction_test.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

basic_test:
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBER_LIBS) $(LIBS) $(BOOST_LIB)

clean:
	rm -rf *.dSYM  *.o
	rm -f segment_test
	rm -f node_test
	rm -f end_to_end
	rm -f seg_time
	rm -f job_test
	rm -f fs_test
	rm -f inst_test
	rm -f rsync_test_suite
	rm -f basic_test

