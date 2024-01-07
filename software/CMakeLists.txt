cmake_minimum_required(VERSION 3.15)
###############################################################################
# CMake Project Setup
###############################################################################
set(CMAKE_SYSTEM_NAME Generic)
project(ahriman)

set(CMAKE_VERBOSE_MAKEFILE ON)

###############################################################################
# CMake Build Variables
###############################################################################
set(COMPILER_SET_UP_FILE cmake/linux.cmake)
set(PROJECT_TOOLS)
set(USED_LANGUAGES C ASM CXX)
set(PROJECT_LIBRARIES bootloader utility abstraction lib/CP2110)
set(PROJECT_EXECUTABLE ${PROJECT_NAME} CACHE INTERNAL "")

###############################################################################
# Determine compiler used and set languages
###############################################################################
include(${COMPILER_SET_UP_FILE})
enable_language(${USED_LANGUAGES})

###############################################################################
# Add Executable and link corresponding libraries
###############################################################################
add_executable(${PROJECT_EXECUTABLE} main.cpp)
target_include_directories(${PROJECT_EXECUTABLE} PUBLIC
    .)

###############################################################################
# Add required libraries
###############################################################################
foreach(LIBRARY ${PROJECT_LIBRARIES})
    add_subdirectory(${CMAKE_SOURCE_DIR}/${LIBRARY}
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY})
endforeach()
target_link_libraries(${PROJECT_EXECUTABLE}
    BOOTLOADER)

###############################################################################
# Run any extra tools that may be required post compilation
###############################################################################
#include(${PROJECT_TOOLS})
