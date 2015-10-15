cmake_minimum_required(VERSION 2.8)
project( libcoral_rsync )

find_package( Boost 1.46 REQUIRED )

set( LIBNAME rsync )
add_definitions ("-Wall")
add_definitions ("-Wno-unused-local-typedefs")

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )

set( INCLUDE_ROOT "../../includes" )
set( CORE_INCLUDE "${INCLUDE_ROOT}/core" )
set( NETAPP_INCLUDE "${INCLUDE_ROOT}/netapp" )
include_directories( "${CORE_INCLUDE}" )
include_directories( "${CORE_INCLUDE}/crypto" )
include_directories( "${CORE_INCLUDE}/file_system" )
include_directories( "${CORE_INCLUDE}/cli" )
include_directories( "${CORE_INCLUDE}/helpers" )
include_directories( "${CORE_INCLUDE}/container" )
include_directories( "${CORE_INCLUDE}/concurrency" )
include_directories( "${CORE_INCLUDE}/time" )
include_directories( "${CORE_INCLUDE}/logging" )
include_directories( "${CORE_INCLUDE}/net" )
include_directories( "${NETAPP_INCLUDE}/common" )
include_directories( "${INCLUDE_ROOT}/rsync" )

set( SOURCE_ROOT "../../src" )
set( COMMON_SOURCE "${SOURCE_ROOT}/common")
set( SOURCES
"${COMMON_SOURCE}/${LIBNAME}/ResourcePath.cpp"
"${COMMON_SOURCE}/${LIBNAME}/JobDescriptor.cpp"
"${COMMON_SOURCE}/${LIBNAME}/JobReport.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RemoteJobResult.cpp"
"${COMMON_SOURCE}/${LIBNAME}/FileSystemInterface.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncPacketRouter.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncJob.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Segment.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncPacket.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncPacketSubscriber.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Assembler.cpp"
"${COMMON_SOURCE}/${LIBNAME}/AssemblerThread.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Authority.cpp"
"${COMMON_SOURCE}/${LIBNAME}/LocalAuthorityInterface.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RemoteAuthorityInterface.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RemoteAuthorityService.cpp"
"${COMMON_SOURCE}/${LIBNAME}/AuthorityThread.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Instruction.cpp"
"${COMMON_SOURCE}/${LIBNAME}/InstructionReceiver.cpp"
"${COMMON_SOURCE}/${LIBNAME}/InstructionList.cpp"
"${COMMON_SOURCE}/${LIBNAME}/InstructionQueue.cpp"
"${COMMON_SOURCE}/${LIBNAME}/InstructionHook.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Segmenter.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmenterThread.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentAccessor.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentHook.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentReceiver.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentFile.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentList.cpp"
"${COMMON_SOURCE}/${LIBNAME}/SegmentQueue.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncNode.cpp"
"${COMMON_SOURCE}/${LIBNAME}/JobAgent.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RsyncQueryResponse.cpp"
"${COMMON_SOURCE}/${LIBNAME}/Worker.cpp"
"${COMMON_SOURCE}/${LIBNAME}/WorkerGroup.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RemoteAuthorityServiceThread.cpp"
"${COMMON_SOURCE}/${LIBNAME}/JobRequestCallback.cpp"
)

add_library( coral_rsync STATIC ${SOURCES} )
#add_library( coral_rsync SHARED ${SOURCES} )
