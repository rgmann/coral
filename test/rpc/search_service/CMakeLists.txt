cmake_minimum_required(VERSION 2.8)
project( search_service )

cmake_policy(SET CMP0015 NEW)
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

# GPBUFLIB = `pkg-config --cflags --libs protobuf` 
# LIBS += $(GPBUFLIB)
#

find_package( Boost 1.46 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )
find_package( Protobuf REQUIRED )

set( INCLUDE_BASE "../../../includes" )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )

include_directories( "../../third_party/gtest-1.7.0/include" )
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
include_directories( "${INCLUDE_BASE}/rpc" )

include_directories( "common/rpc_include" )
include_directories( "client/rpc_include" )
include_directories( "server/rpc_include" )

link_directories( ../../../lib/base )
link_directories( ../../../lib/netapp )
link_directories( ../../../lib/rpc )
link_directories( "../../../third_party/gtest-1.7.0/" )

add_executable( search_service
"common/rpc_source/tutorial.pb.cc"
"client/rpc_source/SearchServiceClientStub.cpp"
"server/rpc_source/SearchServiceServerStub.cpp"
"server/rpc_source/SearchServiceWrapper.cpp"
"server/rpc_source/user.SearchServiceWrapper.cpp"
search_service.cpp )

target_link_libraries( search_service rpc )
target_link_libraries( search_service base )
target_link_libraries( search_service netapp_common )
target_link_libraries( search_service gtest )
target_link_libraries( search_service ${Boost_LIBRARIES} )
target_link_libraries( search_service ${PROTOBUF_LIBRARIES} )
target_link_libraries( search_service pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( search_service rt )
endif()
