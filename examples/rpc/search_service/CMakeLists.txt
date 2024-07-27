cmake_minimum_required(VERSION 3.30)

if (POLICY CMP0015)
  cmake_policy( SET CMP0015 NEW )
endif (POLICY CMP0015)

if (POLICY CMP0042)
  cmake_policy( SET CMP0042 NEW )
endif (POLICY CMP0042)

set( TARGET search_service )
project( ${TARGET} )

add_definitions( "-Wall -g" )
add_definitions( "-Wno-unused-local-typedefs" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.47 REQUIRED system filesystem thread)
find_package( Protobuf REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )

set( INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../includes )
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

set( LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib )
link_directories( ${LIB_ROOT}/core )
link_directories( ${LIB_ROOT}/netapp )
link_directories( ${LIB_ROOT}/rpc )

add_executable( ${TARGET}
${CMAKE_CURRENT_SOURCE_DIR}/common/rpc_source/tutorial.pb.cc
${CMAKE_CURRENT_SOURCE_DIR}/client/rpc_source/SearchServiceClientStub.cpp
${CMAKE_CURRENT_SOURCE_DIR}/server/rpc_source/SearchServiceServerStub.cpp
${CMAKE_CURRENT_SOURCE_DIR}/server/rpc_source/SearchServiceSearchAction.cpp
${TARGET}.cpp )

# Pre-build step: generate all RPC headers and source base on the
# service definition.
add_custom_command(
   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpc_gen_temp/tutorial.pb.cc
          ${CMAKE_CURRENT_SOURCE_DIR}/rpc_gen_temp/SearchServiceClientStub.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/rpc_gen_temp/SearchServiceServerStub.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/rpc_gen_temp/SearchServiceSearchAction.cpp
   COMMAND rake generate
   DEPENDS Rakefile
)
add_custom_command(
   OUTPUT   ${CMAKE_CURRENT_SOURCE_DIR}/common/rpc_source/tutorial.pb.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/client/rpc_source/SearchServiceClientStub.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/server/rpc_source/SearchServiceServerStub.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/server/rpc_source/SearchServiceSearchAction.cpp
   COMMAND  rake situate
   DEPENDS  rpc_gen_temp/tutorial.pb.cc
            rpc_gen_temp/SearchServiceClientStub.cpp
            rpc_gen_temp/SearchServiceServerStub.cpp
            rpc_gen_temp/SearchServiceSearchAction.cpp
            Rakefile
)
add_custom_target(
   RunGenerator
   DEPENDS  rpc_gen_temp/tutorial.pb.cc
            rpc_gen_temp/SearchServiceClientStub.cpp
            rpc_gen_temp/SearchServiceServerStub.cpp
            rpc_gen_temp/SearchServiceSearchAction.cpp 
   COMMENT "Checking if re-generation is required"
)

add_dependencies( ${TARGET} RunGenerator )


# Remove all auto-generated RPC files
add_custom_target(
   distclean
   COMMAND rm  client/rpc_include/*.h
               client/rpc_source/*.cpp
               server/rpc_include/*.h
               server/rpc_source/*.cpp
               common/rpc_include/*.h
               common/rpc_source/*.cc
)


target_link_libraries( ${TARGET} coral_rpc_shared )
target_link_libraries( ${TARGET} ${Boost_LIBRARIES} )
target_link_libraries( ${TARGET} ${PROTOBUF_LIBRARIES} )
target_link_libraries( ${TARGET} pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( ${TARGET} rt )
endif()
