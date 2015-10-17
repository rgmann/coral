cmake_minimum_required( VERSION 2.8 )

#
# Build all libraries
#
add_subdirectory( lib/core )
add_subdirectory( lib/netapp )
add_subdirectory( lib/rpc )
add_subdirectory( lib/rsync )

#
# Build tests
#
add_subdirectory( test/core )
add_subdirectory( test/netapp )
# add_subdirectory( test/rpc )
add_subdirectory( test/rsync )

#
# Build the RPC code generator
#
add_subdirectory( utils/rpc_generator )
