cmake_minimum_required(VERSION 2.8)
project( protoc-gen-libergen )

cmake_policy(SET CMP0015 NEW)
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")


find_package( Protobuf REQUIRED )

set( INCLUDE_BASE "../../includes" )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )

include_directories( "${INCLUDE_BASE}/base" )
include_directories( "${INCLUDE_BASE}/base/file_system" )
include_directories( "${INCLUDE_BASE}/base/cli" )
include_directories( "${INCLUDE_BASE}/base/helpers" )
include_directories( "${INCLUDE_BASE}/base/container" )


link_directories( "../../lib/base" )

add_executable( protoc-gen-libergen
LiberFileGenerator.cpp
libergen.cpp )

target_link_libraries( protoc-gen-libergen base )
target_link_libraries( protoc-gen-libergen ${PROTOBUF_LIBRARIES} )
target_link_libraries( protoc-gen-libergen protoc )
target_link_libraries( protoc-gen-libergen pthread )

if(APPLE)
elseif(UNIX)
  # target_link_libraries( protoc-gen-libergen rt )
endif()
