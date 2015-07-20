SHELL = /bin/bash
CC = g++
# CC = clang++
CFLAGS = -Wall -Wno-deprecated-register -c -g -O1
LIBNAME = rsync
BASE = ../..

BOOST_INCLUDE = \
-I/opt/local/include \

SOURCE = \
$(BASE)/src/common/$(LIBNAME)/JobDescriptor.cpp \
$(BASE)/src/common/$(LIBNAME)/JobReport.cpp \
$(BASE)/src/common/$(LIBNAME)/RemoteJobResult.cpp \
$(BASE)/src/common/$(LIBNAME)/FileSystemInterface.cpp \
$(BASE)/src/common/$(LIBNAME)/RsyncPacketRouter.cpp \
$(BASE)/src/common/$(LIBNAME)/RsyncJob.cpp \
$(BASE)/src/common/$(LIBNAME)/Segment.cpp \
$(BASE)/src/common/$(LIBNAME)/RsyncPacket.cpp \
$(BASE)/src/common/$(LIBNAME)/Assembler.cpp \
$(BASE)/src/common/$(LIBNAME)/AssemblerThread.cpp \
$(BASE)/src/common/$(LIBNAME)/Authority.cpp \
$(BASE)/src/common/$(LIBNAME)/LocalAuthorityInterface.cpp \
$(BASE)/src/common/$(LIBNAME)/RemoteAuthorityInterface.cpp \
$(BASE)/src/common/$(LIBNAME)/RemoteAuthorityService.cpp \
$(BASE)/src/common/$(LIBNAME)/AuthorityThread.cpp \
$(BASE)/src/common/$(LIBNAME)/Instruction.cpp \
$(BASE)/src/common/$(LIBNAME)/InstructionReceiver.cpp \
$(BASE)/src/common/$(LIBNAME)/InstructionList.cpp \
$(BASE)/src/common/$(LIBNAME)/InstructionQueue.cpp \
$(BASE)/src/common/$(LIBNAME)/InstructionHook.cpp \
$(BASE)/src/common/$(LIBNAME)/Segmenter.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmenterThread.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentAccessor.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentHook.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentReceiver.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentFile.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentList.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentQueue.cpp \
$(BASE)/src/common/$(LIBNAME)/SegmentHash.cpp \
$(BASE)/src/common/$(LIBNAME)/RsyncNode.cpp \
$(BASE)/src/common/$(LIBNAME)/JobAgent.cpp \

INCLUDES = $(BOOST_INCLUDE) \
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
-I$(BASE)/includes/rsync \


$(LIBNAME):
	$(CC) $(CFLAGS) $(SOURCE) $(INCLUDES) -std=c++11 -stdlib=libc++
	ar -cvq lib$(LIBNAME).a *.o

all: $(LIBNAME)

clean:
	rm -rf  *.o *.a
