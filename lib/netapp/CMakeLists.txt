cmake_minimum_required(VERSION 2.8)
project( libnetapp )

find_package( Boost 1.47 REQUIRED )
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )

#
# Common include paths
#
set( INCLUDE_ROOT "../../includes" )
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


set( SOURCE_ROOT "../../src" )
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

add_library( netapp_common STATIC ${COMMON_SOURCE} )
add_library( netapp_server STATIC ${SERVER_SOURCE} )
add_library( netapp_client STATIC ${CLIENT_SOURCE} )

target_include_directories( netapp_server PUBLIC "${NETAPP_INCLUDE}/server" )
target_include_directories( netapp_client PUBLIC "${NETAPP_INCLUDE}/client" )
