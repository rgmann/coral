SHELL = /bin/bash
CC = g++
CFLAGS = -Wall -c -g
GTEST_DIR = ../../third_party/gtest-1.7.0

gtest_lib:
	$(CC) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

netapp_test: gtest_lib
	$(CC) -isystem ${GTEST_DIR}/include -pthread netapp_server_test.cpp libgtest.a -o netapp_server_test

clean:
	rm -f *.a *.o *.dSYM

