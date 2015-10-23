cmake_minimum_required(VERSION 2.8)

set( LIBNAME coral_rpc )
project( lib${LIBNAME} )

find_package( Boost 1.47 REQUIRED )


if ( TARGET ${LIBNAME} )

  message("${LIBNAME} is already defined")

else()

add_definitions ("-Wall -g")
add_definitions ("-Wno-unused-local-typedefs")

set( LIB_SRC_DIR rpc )

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
"${COMMON_SOURCE}/${LIB_SRC_DIR}/AsyncRpcSupervisor.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcPacket.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/BlockingRpcSupervisor.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcClient.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcClientResource.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcException.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcMarshalledCall.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcObject.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcServer.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcServerResource.cpp"
"${COMMON_SOURCE}/${LIB_SRC_DIR}/RpcSupervisor.cpp"
)

add_library( ${LIBNAME} STATIC ${SOURCES} )

endif()
