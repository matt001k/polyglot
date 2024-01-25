cmake_minimum_required(VERSION 3.15)
###############################################################################
# CMake Project Setup
###############################################################################
set(CMAKE_SYSTEM_NAME Generic)
project(ahriman)
message("CMAKE_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

#set(CMAKE_VERBOSE_MAKEFILE ON)

###############################################################################
# CMake Build Variables
###############################################################################
set(COMPILER_SET_UP_FILE shared/platform/efm32/efm32.cmake)
set(PROJECT_TOOLS cmake/tools.cmake)
set(USED_LANGUAGES C ASM)
set(PROJECT_LIBRARIES application
                      firmware
                      shared)
set(PROJECT_APP_NAME ${PROJECT_NAME}_app)
set(PROJECT_BOOT_NAME ${PROJECT_NAME}_boot)
set(PROJECT_APP_EXECUTABLE ${PROJECT_APP_NAME}.elf CACHE INTERNAL "")
set(PROJECT_BOOT_EXECUTABLE ${PROJECT_BOOT_NAME}.elf CACHE INTERNAL "")

###############################################################################
# Add Executable to link corresponding libraries
###############################################################################
add_executable(${PROJECT_APP_EXECUTABLE})
add_executable(${PROJECT_BOOT_EXECUTABLE})

###############################################################################
# Determine compiler used and set languages
###############################################################################
include(${COMPILER_SET_UP_FILE})
enable_language(${USED_LANGUAGES})

###############################################################################
# Add required libraries
###############################################################################
foreach(LIBRARY ${PROJECT_LIBRARIES})
    add_subdirectory(${CMAKE_SOURCE_DIR}/${LIBRARY}
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY})
endforeach()

###############################################################################
# Run any extra tools that may be required post compilation
###############################################################################
include(${PROJECT_TOOLS})
