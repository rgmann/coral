cmake_minimum_required(VERSION 2.8)
project( protoc-gen-libergen )

cmake_policy(SET CMP0015 NEW)
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")


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

add_executable( protoc-gen-libergen
LiberFileGenerator.cpp
libergen.cpp )

target_link_libraries( protoc-gen-libergen liber_core )
target_link_libraries( protoc-gen-libergen ${PROTOBUF_LIBRARIES} )
target_link_libraries( protoc-gen-libergen protoc )
target_link_libraries( protoc-gen-libergen pthread )

if(APPLE)
elseif(UNIX)
  # target_link_libraries( protoc-gen-libergen rt )
endif()
