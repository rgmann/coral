cmake_minimum_required(VERSION 3.30)

set( TEST_TARGET core_test_suite )
project( ${TEST_TARGET} )

if (POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif (POLICY CMP0015)

add_definitions( "-Wall" )
add_definitions( "-Wno-unused-local-typedefs" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.47 REQUIRED system filesystem thread)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories( ${Boost_INCLUDE_DIRS} )
include(GoogleTest)

set( INCLUDE_ROOT "../../includes" )
set( CORE_INCLUDE "${INCLUDE_ROOT}/core" )
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


link_directories( ../../lib/core )

add_executable( ${TEST_TARGET} ${TEST_TARGET}.cpp )

target_link_libraries( ${TEST_TARGET} coral_core )
target_link_libraries( ${TEST_TARGET} GTest::gtest_main)
target_link_libraries( ${TEST_TARGET} ${Boost_LIBRARIES} )
target_link_libraries( ${TEST_TARGET} pthread )

if(APPLE)
elseif(UNIX)
  target_link_libraries( ${TEST_TARGET} rt )
endif()
