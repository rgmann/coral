cmake_minimum_required(VERSION 2.8)
project( rsync_test )

set( GTEST_ROOT "/Users/vaughanbiker/Development/gtest-1.7.0" )

cmake_policy(SET CMP0015 NEW)
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

find_package( Boost 1.46 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )

enable_testing()
find_package(GTest REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${GTEST_INCLUDE_DIRS} )

set( INCLUDE_BASE "../../includes" )
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
include_directories( "${INCLUDE_BASE}/rsync" )

link_directories( ../../lib/base )
link_directories( ../../lib/netapp )
link_directories( ../../lib/rsync )

add_executable( rsync_test_suite rsync_test_suite.cpp )

target_link_libraries( rsync_test_suite rsync )
target_link_libraries( rsync_test_suite base )
target_link_libraries( rsync_test_suite netapp_common )
target_link_libraries( rsync_test_suite ${GTEST_BOTH_LIBRARIES})
target_link_libraries( rsync_test_suite ${Boost_LIBRARIES} )
target_link_libraries( rsync_test_suite pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( rsync_test_suite rt )
endif()
