cmake_minimum_required(VERSION 2.8)

set( TEST_TARGET netapp_test_suite )
project( ${TEST_TARGET} )

if (POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif (POLICY CMP0015)

find_package( Boost 1.47 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )

enable_testing()
find_package(GTest REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${GTEST_INCLUDE_DIRS} )

set( INCLUDE_ROOT "../../includes" )
set( CORE_INCLUDE "../../includes/core" )
set( NETAPP_INCLUDE "../../includes/netapp" )
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

link_directories( ../../lib/core )
link_directories( ../../lib/netapp )

add_executable( ${TEST_TARGET} ${TEST_TARGET}.cpp )

target_link_libraries( ${TEST_TARGET} coral_netapp_common_shared )
target_link_libraries( ${TEST_TARGET} coral_core_shared )
target_link_libraries( ${TEST_TARGET} ${GTEST_BOTH_LIBRARIES})
target_link_libraries( ${TEST_TARGET} ${Boost_LIBRARIES} )
target_link_libraries( ${TEST_TARGET} pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( ${TEST_TARGET} rt )
endif()
