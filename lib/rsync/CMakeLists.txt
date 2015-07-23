cmake_minimum_required(VERSION 2.8)
project( librsync )

find_package( Boost 1.48 REQUIRED )

set( LIBNAME rsync )

set( INCLUDE_BASE "../../includes" )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( "${INCLUDE_BASE}/base" )
include_directories( "${INCLUDE_BASE}/base/crypto" )
include_directories( "${INCLUDE_BASE}/base/file_system" )
include_directories( "${INCLUDE_BASE}/base/cli" )
include_directories( "${INCLUDE_BASE}/base/helpers" )
include_directories( "${INCLUDE_BASE}/base/container" )
include_directories( "${INCLUDE_BASE}/base/concurrency" )
include_directories( "${INCLUDE_BASE}/base/time" )
include_directories( "${INCLUDE_BASE}/base/logging" )
include_directories( "${INCLUDE_BASE}/base/net" )
include_directories( "${INCLUDE_BASE}/netapp/common" )
include_directories( "${INCLUDE_BASE}/rsync" )

set( SOURCE_BASE "../.." )
set( SOURCES
"${SOURCE_BASE}/src/common/${LIBNAME}/JobDescriptor.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/JobReport.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RemoteJobResult.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/FileSystemInterface.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RsyncPacketRouter.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RsyncJob.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/Segment.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RsyncPacket.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/Assembler.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/AssemblerThread.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/Authority.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/LocalAuthorityInterface.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RemoteAuthorityInterface.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RemoteAuthorityService.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/AuthorityThread.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/Instruction.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/InstructionReceiver.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/InstructionList.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/InstructionQueue.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/InstructionHook.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/Segmenter.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmenterThread.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentAccessor.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentHook.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentReceiver.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentFile.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentList.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentQueue.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/SegmentHash.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RsyncNode.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/JobAgent.cpp"
)

add_library( rsync STATIC ${SOURCES} )
