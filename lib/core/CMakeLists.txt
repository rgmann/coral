cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0042)
  cmake_policy( SET CMP0042 NEW )
endif (POLICY CMP0042)

project( libcoral_core )

find_package( Boost 1.47 REQUIRED system-mt filesystem-mt thread-mt )

include_directories( ${Boost_INCLUDE_DIRS} )

set( INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../includes/core )
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

set( NIX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/nix/core )
set( COMMON_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/common/core )
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

add_library( coral_core_shared SHARED ${SOURCES} )
target_link_libraries( coral_core_shared ${Boost_LIBRARIES} )
