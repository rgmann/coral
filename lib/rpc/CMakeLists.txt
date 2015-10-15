cmake_minimum_required(VERSION 2.8)
project( libcoral_rpc )

find_package( Boost 1.46 REQUIRED )

set( LIBNAME rpc )
add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )


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
include_directories( "${INCLUDE_ROOT}/${LIBNAME}" )

set( SOURCE_ROOT "../.." )
set( COMMON_SOURCE "${SOURCE_ROOT}/src/common" )
set( SOURCES
"${COMMON_SOURCE}/${LIBNAME}/AsyncRpcSupervisor.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcPacket.cpp"
"${COMMON_SOURCE}/${LIBNAME}/BlockingRpcSupervisor.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcClient.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcClientResource.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcException.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcMarshalledCall.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcObject.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcServer.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcServerResource.cpp"
"${COMMON_SOURCE}/${LIBNAME}/RpcSupervisor.cpp"
)

add_library( libcoral_rpc STATIC ${SOURCES} )
