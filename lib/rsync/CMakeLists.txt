cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0042)
  cmake_policy( SET CMP0042 NEW )
endif (POLICY CMP0042)

set( LIBNAME coral_rsync )
project( lib${LIBNAME} )

find_package( Boost 1.47 REQUIRED system-mt filesystem-mt thread-mt )

set( LIB_SRC_DIR rsync )


if ( TARGET ${LIBNAME} )

  message("${LIBNAME} is already defined")

else()

add_definitions ("-Wall")
add_definitions ("-Wno-unused-local-typedefs")

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )

set( INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../includes )
set( CORE_INCLUDE ${INCLUDE_ROOT}/core )
set( NETAPP_INCLUDE ${INCLUDE_ROOT}/netapp )
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

set( SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../src )
set( COMMON_SOURCE ${SOURCE_ROOT}/common )
set( SOURCES
"${COMMON_SOURCE}/${LIB_SRC_DIR}/ResourcePath.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/JobDescriptor.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/JobReport.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RemoteJobResult.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/FileSystemInterface.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncPacketRouter.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncJob.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Segment.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncPacket.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncPacketSubscriber.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Assembler.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/AssemblerThread.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Authority.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/LocalAuthorityInterface.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RemoteAuthorityInterface.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RemoteAuthorityService.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/AuthorityThread.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Instruction.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/InstructionReceiver.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/InstructionList.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/InstructionQueue.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/InstructionHook.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Segmenter.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmenterThread.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentAccessor.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentHook.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentReceiver.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentFile.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentList.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/SegmentQueue.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncNode.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/JobAgent.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RsyncQueryResponse.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/Worker.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/WorkerGroup.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RemoteAuthorityServiceThread.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/JobRequestCallback.cpp"
)

link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../core )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../netapp )

add_library( ${LIBNAME} STATIC ${SOURCES} )

add_library( ${LIBNAME}_shared SHARED ${SOURCES} )
target_link_libraries(
  ${LIBNAME}_shared ${Boost_LIBRARIES}
  coral_core
  coral_netapp_common_shared )

endif()
