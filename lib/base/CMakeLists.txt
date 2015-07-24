cmake_minimum_required(VERSION 2.8)
project( libbase )

find_package( Boost 1.46 REQUIRED )

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

set( SOURCE_BASE "../../src" )
file( GLOB SOURCES
"${SOURCE_BASE}/nix/base/*.cpp"
"${SOURCE_BASE}/nix/base/file_system/*.cpp"
"${SOURCE_BASE}/nix/base/crypto/*.cpp"
"${SOURCE_BASE}/nix/base/concurrency/*.cpp"
"${SOURCE_BASE}/nix/base/time/*.cpp"
"${SOURCE_BASE}/nix/base/net/*.cpp"
"${SOURCE_BASE}/common/base/cli/*.cpp"
"${SOURCE_BASE}/common/base/helpers/*.cpp"
"${SOURCE_BASE}/common/base/container/*.cpp"
"${SOURCE_BASE}/common/base/logging/*.cpp"
"${SOURCE_BASE}/common/base/file_system/*.cpp"
"${SOURCE_BASE}/common/base/net/*.cpp"
"${SOURCE_BASE}/common/base/concurrency/*.cpp"
)

add_library( base STATIC ${SOURCES} )
