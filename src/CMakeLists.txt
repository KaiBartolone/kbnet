# Add Source Files
file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/*  ${KBTHREAD_LIB_DIR}/* )

# Add Library
add_library( kbnet STATIC ${SRCFILES} )

# Specify Include Directories
target_include_directories( kbnet PUBLIC ${ASIO_INCLUDE_DIR}  ${KBTHREAD_INCLUDE_DIR} )