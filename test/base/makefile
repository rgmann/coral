
CC = gcc
INC =	-I../../includes/base \
	-I../../includes/base/cli \
	-I../../includes/base/hash \
	-I../../includes/base/crypto \
	-I../../includes/base/file_system \
	-I../../includes/base/helpers \
	-I../../includes/base/container \
	-I../../includes/base/concurrency \
	-I../../includes/base/logging \
	-I../../includes/base/time \
	-I../../includes/base/net
ABSLIB = -L../../lib/base -lbase
LIBS = -lstdc++ -lpthread

BOOST_LIB_DIR = -L/opt/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_filesystem-mt
BOOST_LIB = $(BOOST_LIB_DIR) $(BOOST_LIBS)

allocator:
	$(CC) -Wall -g -o allocator allocator_test.cpp $(INC) $(ABSLIB) $(LIBS)

functor:
	$(CC) -Wall -g -o functor functor_test.cpp $(INC) $(ABSLIB) $(LIBS)

base_types:
	$(CC) -Wall -g -o base_types base_type_test.cpp $(INC) $(ABSLIB) $(LIBS)

byte_order:
	$(CC) -Wall -g -o byte_order byte_order_test.cpp $(INC) $(ABSLIB) $(LIBS)

client:
	$(CC) -Wall -g -o client testclient.cpp $(INC) $(ABSLIB) $(LIBS)

server:
	$(CC) -Wall -g -o server testserver.cpp $(INC) $(ABSLIB) $(LIBS)

thread:
	$(CC) -Wall -g -o thread threadtest.cpp $(INC) $(ABSLIB) $(LIBS)

ithread:
	$(CC) -Wall -g -o ithread ithread_test.cpp $(INC) $(ABSLIB) $(LIBS)

sem:
	$(CC) -Wall -g -o sem semtest.cpp $(INC) $(ABSLIB) $(LIBS)

queue:
	$(CC) -Wall -g -o queue queuetest.cpp $(INC) $(ABSLIB) $(LIBS)
	
fs:
	$(CC) -Wall -g -o fs fstest.cpp $(INC) $(ABSLIB) $(LIBS)

mon:
	$(CC) -Wall -g -o mon montest.cpp $(INC) $(ABSLIB) $(LIBS)

dsv:
	$(CC) -Wall -g -o dsv dsvtest.cpp $(INC) $(ABSLIB) $(LIBS)

rsync:
	$(CC) -Wall -g -o rsync rsynctest.cpp $(INC) $(ABSLIB) $(LIBS)
	
arg:
	$(CC) -Wall -g -o arg argtest.cpp $(INC) $(ABSLIB) $(LIBS)

hashtab:
	$(CC) -Wall -g -o hashtab hashtest.cpp $(INC) $(ABSLIB) $(LIBS)

hash:
	$(CC) -Wall -g -o $@ hash_table_test.cpp $(INC) $(ABSLIB) $(LIBS)

segm:
	$(CC) -Wall -g -o segm segmentertest.cpp $(INC) $(ABSLIB) $(LIBS)

log:
	$(CC) -Wall -g -o log logtest.cpp $(INC) $(ABSLIB) $(LIBS)

logtest:
	$(CC) -Wall -g -o logtest log_test_new.cpp OtherLogUser.cpp $(INC) $(ABSLIB) $(LIBS) $(BOOST_LIB)

bufftest:
	$(CC) -Wall -g -o bufftest circ_buff_test.cpp $(INC) $(ABSLIB) $(LIBS) $(BOOST_LIB)


ts:
	$(CC) -Wall -g -o ts timestamp_test.cpp $(INC) $(ABSLIB) $(LIBS)

derived:
	$(CC) -Wall -g -o derived derived_test.cpp $(INC) $(ABSLIB) $(LIBS)

remove_if:
	$(CC) -Wall -g -o remove_if remove_if_test.cpp $(INC) $(ABSLIB) $(LIBS)


clean:
	rm -rf *.dSYM  *.o
	rm -f allocator
	rm -f derived
	rm -f functor
	rm -f logtest
	rm -f hash
	rm -f remove_if
	rm -f bufftest
	rm -f ts ithread log base_types client server thread sem queue fs mon dsv rsync arg hashtab segm byte_order
