cmake_minimum_required( VERSION 3.13 )
project(coral VERSION 1.1.0)

#
# Build all libraries
#
add_subdirectory( lib/core )
add_subdirectory( lib/netapp )

#
# Build tests
#
add_subdirectory( test/core )
add_subdirectory( test/netapp )

