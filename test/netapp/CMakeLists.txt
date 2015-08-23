cmake_minimum_required(VERSION 2.8)
project( netapp_test )

cmake_policy(SET CMP0015 NEW)

find_package( Boost 1.46 REQUIRED COMPONENTS system-mt filesystem-mt thread-mt )

set( INCLUDE_BASE "../../includes" )
include_directories( ${Boost_INCLUDE_DIRS} )
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

link_directories( ../../lib/base )
link_directories( ../../lib/netapp )
link_directories( "../../third_party/gtest-1.7.0/" )

add_executable( netapp_test_suite netapp_test_suite.cpp )

target_link_libraries( netapp_test_suite base )
target_link_libraries( netapp_test_suite netapp_common )
target_link_libraries( netapp_test_suite gtest )
target_link_libraries( netapp_test_suite ${Boost_LIBRARIES} )
target_link_libraries( netapp_test_suite pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( netapp_test_suite rt )
endif()