cmake_minimum_required(VERSION 2.8)
project( libcoral_core )

find_package( Boost 1.47 REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )

set( INCLUDE_ROOT "../../includes/core" )
include_directories( "${INCLUDE_ROOT}" )
include_directories( "${INCLUDE_ROOT}/crypto" )
include_directories( "${INCLUDE_ROOT}/file_system" )
include_directories( "${INCLUDE_ROOT}/cli" )
include_directories( "${INCLUDE_ROOT}/helpers" )
include_directories( "${INCLUDE_ROOT}/container" )
include_directories( "${INCLUDE_ROOT}/concurrency" )
include_directories( "${INCLUDE_ROOT}/time" )
include_directories( "${INCLUDE_ROOT}/logging" )
include_directories( "${INCLUDE_ROOT}/net" )

set( NIX_SOURCE "../../src/nix/core" )
set( COMMON_SOURCE "../../src/common/core" )
file( GLOB SOURCES
"${NIX_SOURCE}/file_system/*.cpp"
"${NIX_SOURCE}/crypto/*.cpp"
"${NIX_SOURCE}/time/*.cpp"
"${NIX_SOURCE}/net/*.cpp"
"${COMMON_SOURCE}/concurrency/IThread.cpp"
"${COMMON_SOURCE}/cli/*.cpp"
"${COMMON_SOURCE}/helpers/*.cpp"
"${COMMON_SOURCE}/container/*.cpp"
"${COMMON_SOURCE}/logging/*.cpp"
"${COMMON_SOURCE}/file_system/*.cpp"
"${COMMON_SOURCE}/net/*.cpp"
"${COMMON_SOURCE}/concurrency/*.cpp"
)

add_library( coral_core STATIC ${SOURCES} )
