cmake_minimum_required(VERSION 3.30)


set( TARGET_NAME protoc-gen-coralrpc )
project( ${TARGET_NAME} )

if (POLICY CMP0015)
  cmake_policy( SET CMP0015 NEW )
endif (POLICY CMP0015)

add_definitions( "-Wall -g" )
add_definitions( "-Wno-unused-local-typedefs" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.47 REQUIRED system filesystem thread)
include_directories( ${Boost_INCLUDE_DIRS} )

find_package( Protobuf REQUIRED )
include_directories( ${Protobuf_INCLUDE_DIRS} )

set( INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../includes )
set( CORE_INCLUDE "${INCLUDE_ROOT}/core")
#include_directories( ${PROTOBUF_INCLUDE_DIRS} )

include_directories( "${CORE_INCLUDE}" )
include_directories( "${CORE_INCLUDE}/file_system" )
include_directories( "${CORE_INCLUDE}/cli" )
include_directories( "${CORE_INCLUDE}/helpers" )
include_directories( "${CORE_INCLUDE}/container" )


link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/core )

add_executable(
  ${TARGET_NAME}
  RpcServiceGenerator.cpp
  rpc_generator.cpp
)

target_link_libraries( ${TARGET_NAME} coral_core_shared )
target_link_libraries( ${TARGET_NAME} ${Protobuf_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} ${Protobuf_PROTOC_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} pthread )
