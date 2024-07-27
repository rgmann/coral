cmake_minimum_required(VERSION 3.30)
project( libcoral_netapp )

if (POLICY CMP0042)
  cmake_policy( SET CMP0042 NEW )
endif (POLICY CMP0042)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.47 REQUIRED system filesystem thread)
add_definitions( "-Wall -g" )
add_definitions( "-Wno-unused-local-typedefs" )

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )

#
# Common include paths
#
set( INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../includes )
set( CORE_INCLUDE "${INCLUDE_ROOT}/core" )
set( NETAPP_INCLUDE "${INCLUDE_ROOT}/netapp" )
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
include_directories( "${NETAPP_INCLUDE}/client" )
include_directories( "${NETAPP_INCLUDE}/server" )



set( SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../src )
file( GLOB COMMON_SOURCE
"${SOURCE_ROOT}/common/netapp/common/*.cpp"
)
file( GLOB SERVER_SOURCE
"${SOURCE_ROOT}/common/netapp/common/*.cpp"
"${SOURCE_ROOT}/common/netapp/server/*.cpp"
)
file( GLOB CLIENT_SOURCE
"${SOURCE_ROOT}/common/netapp/common/*.cpp"
"${SOURCE_ROOT}/common/netapp/client/*.cpp"
)

add_library( coral_netapp_common STATIC ${COMMON_SOURCE} )
add_library( coral_netapp_server STATIC ${SERVER_SOURCE} )
add_library( coral_netapp_client STATIC ${CLIENT_SOURCE} )


add_library( coral_netapp_common_shared SHARED ${COMMON_SOURCE} )
target_link_libraries(
  coral_netapp_common_shared
  ${Boost_LIBRARIES}
  coral_core_shared )

add_library( coral_netapp_server_shared SHARED ${SERVER_SOURCE} )
target_link_libraries(
  coral_netapp_server_shared
  ${Boost_LIBRARIES}
  coral_core_shared )

add_library( coral_netapp_client_shared SHARED ${CLIENT_SOURCE} )
target_link_libraries(
  coral_netapp_client_shared
  ${Boost_LIBRARIES}
  coral_core_shared )

