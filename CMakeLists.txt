# the minimum version of CMake required
cmake_minimum_required( VERSION 3.8 )

# Set Flags
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Set Source Files
set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( KBTHREAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/kbthread/include )
set( KBTHREAD_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/kbthread/libs )
set( ASIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/asio-1.18.2/include )

# Create project
project( kbnet )
add_subdirectory( src )
add_subdirectory( test )