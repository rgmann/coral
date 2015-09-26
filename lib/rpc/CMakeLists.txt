cmake_minimum_required(VERSION 2.8)
project( librpc )

find_package( Boost 1.46 REQUIRED )

set( LIBNAME rpc )
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

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
include_directories( "${INCLUDE_BASE}/rpc" )

set( SOURCE_BASE "../.." )
set( SOURCES
"${SOURCE_BASE}/src/common/${LIBNAME}/AsyncRpcSupervisor.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcPacket.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/BlockingRpcSupervisor.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcClient.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcClientResource.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcException.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcMarshalledCall.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcObject.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcServer.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcServerResource.cpp"
"${SOURCE_BASE}/src/common/${LIBNAME}/RpcSupervisor.cpp"
)

add_library( rpc STATIC ${SOURCES} )
