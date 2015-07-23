cmake_minimum_required(VERSION 2.8)
project( libnetapp )

find_package( Boost 1.46 REQUIRED )

#
# Common include paths
#
set( INCLUDE_BASE "../../includes" )
include_directories( ${Boost_INCLUDE_DIRS} )
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


set( SOURCE_BASE "../../src" )
file( GLOB COMMON_SOURCE
"${SOURCE_BASE}/common/netapp/common/*.cpp"
)
file( GLOB SERVER_SOURCE
"${SOURCE_BASE}/common/netapp/common/*.cpp"
"${SOURCE_BASE}/common/netapp/server/*.cpp"
)
file( GLOB CLIENT_SOURCE
"${SOURCE_BASE}/common/netapp/common/*.cpp"
"${SOURCE_BASE}/common/netapp/client/*.cpp"
)

add_library( netapp_common STATIC ${COMMON_SOURCE} )
add_library( netapp_server STATIC ${SERVER_SOURCE} )
add_library( netapp_client STATIC ${CLIENT_SOURCE} )

target_include_directories( netapp_server PUBLIC "${INCLUDE_BASE}/netapp/server" )
target_include_directories( netapp_client PUBLIC "${INCLUDE_BASE}/netapp/client" )
