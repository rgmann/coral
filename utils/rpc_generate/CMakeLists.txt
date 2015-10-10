cmake_minimum_required(VERSION 2.8)


set( TARGET_NAME protoc-gen-coralrpc )
project( ${TARGET_NAME} )

cmake_policy( SET CMP0015 NEW )
add_definitions( "-Wall -g" )
add_definitions( "-Wno-unused-local-typedefs" )


find_package( Protobuf REQUIRED )

set( INCLUDE_ROOT "../../includes" )
set( CORE_INCLUDE "${INCLUDE_ROOT}/core")
include_directories( ${PROTOBUF_INCLUDE_DIRS} )

include_directories( "${CORE_INCLUDE}" )
include_directories( "${CORE_INCLUDE}/file_system" )
include_directories( "${CORE_INCLUDE}/cli" )
include_directories( "${CORE_INCLUDE}/helpers" )
include_directories( "${CORE_INCLUDE}/container" )


link_directories( "../../lib/core" )

add_executable(
  ${TARGET_NAME}
  RpcServiceGenerator.cpp
  rpc_generator.cpp
)

target_link_libraries( ${TARGET_NAME} liber_core )
target_link_libraries( ${TARGET_NAME} ${PROTOBUF_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} protoc )
target_link_libraries( ${TARGET_NAME} pthread )
