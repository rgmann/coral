cmake_minimum_required(VERSION 2.8)

set( TARGET search_service )
project( ${TARGET} )

cmake_policy(SET CMP0015 NEW)
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

find_package( Boost 1.46 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )
find_package( Protobuf REQUIRED )

set( INCLUDE_BASE "../../../includes" )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )

set( INCLUDE_ROOT ../../../includes )
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
include_directories( "${INCLUDE_ROOT}/rpc" )

include_directories( "common/rpc_include" )
include_directories( "client/rpc_include" )
include_directories( "server/rpc_include" )

set( LIB_ROOT ../../../lib )
link_directories( ${LIB_ROOT}/core )
link_directories( ${LIB_ROOT}/netapp )
link_directories( ${LIB_ROOT}/rpc )

add_executable( ${TARGET}
"common/rpc_source/tutorial.pb.cc"
"client/rpc_source/SearchServiceClientStub.cpp"
"server/rpc_source/SearchServiceServerStub.cpp"
"server/rpc_source/SearchServiceSearchAction.cpp"
${TARGET}.cpp )

target_link_libraries( ${TARGET} coral_rpc )
target_link_libraries( ${TARGET} coral_core )
target_link_libraries( ${TARGET} netapp_common )
target_link_libraries( ${TARGET} ${Boost_LIBRARIES} )
target_link_libraries( ${TARGET} ${PROTOBUF_LIBRARIES} )
target_link_libraries( ${TARGET} pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( ${TARGET} rt )
endif()
