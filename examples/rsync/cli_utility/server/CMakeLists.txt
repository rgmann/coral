cmake_minimum_required(VERSION 2.8)
project( rsync_server )

cmake_policy(SET CMP0015 NEW)

find_package( Boost 1.46 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )
include_directories( ${Boost_INCLUDE_DIRS} )

set( BASE_DIR ../../../.. )
set( INCLUDE_ROOT "${BASE_DIR}/includes" )
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
include_directories( "${NETAPP_INCLUDE}/server" )
include_directories( "${INCLUDE_ROOT}/rsync" )

link_directories( ${BASE_DIR}/lib/core )
link_directories( ${BASE_DIR}/lib/netapp )
link_directories( ${BASE_DIR}/lib/rsync )

add_executable( rsync_server rsync_server.cpp )


target_link_libraries( rsync_server coral_rsync )
target_link_libraries( rsync_server netapp_server )
target_link_libraries( rsync_server netapp_common )
target_link_libraries( rsync_server coral_core )
target_link_libraries( rsync_server ${Boost_LIBRARIES})
target_link_libraries( rsync_server pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( rsync_server rt )
endif()
