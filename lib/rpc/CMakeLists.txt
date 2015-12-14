cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0042)
  cmake_policy( SET CMP0042 NEW )
endif (POLICY CMP0042)

set( LIBNAME coral_rpc )
project( lib${LIBNAME} )

find_package( Boost 1.47 REQUIRED system-mt filesystem-mt thread-mt )
find_package( Protobuf REQUIRED )

if ( TARGET ${LIBNAME} )

  message("${LIBNAME} is already defined")

else()

add_definitions( "-Wall -g" )
add_definitions( "-Wno-unused-local-typedefs" )

set( LIB_SRC_DIR rpc )

#
# Third party includes
#
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
message(STATUS "protobuf=${PROTOBUF_INCLUDE_DIRS}")


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
include_directories( "${INCLUDE_ROOT}/${LIB_SRC_DIR}" )

set( SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
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

add_library( ${LIBNAME}_shared SHARED ${SOURCES} )
target_link_libraries( ${LIBNAME}_shared
  ${Boost_LIBRARIES}
  coral_core_shared
  coral_netapp_common_shared
  ${PROTOBUF_LIBRARIES} )

endif()
