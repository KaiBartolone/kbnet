#set vcpkg toolchain
set( CMAKE_TOOLCHAIN_FILE C:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake )

# Set Source Files
file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/* ${SRC_DIR}/* ${KBTHREAD_LIB_DIR}/* )

# Add executable
add_executable( test ${SRCFILES} )

# Find Packages
find_package( GTest CONFIG REQUIRED )

# Specify Include Directories
target_include_directories( test PRIVATE ${GTest_INCLUDE_DIRS} ${SRC_DIR} ${ASIO_INCLUDE_DIR}  ${KBTHREAD_INCLUDE_DIR} )

# Link Dependencies
target_link_libraries( test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main )