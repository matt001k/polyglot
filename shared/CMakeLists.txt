###############################################################################
# Add required libraries
###############################################################################
set(SHARED_LIBRARIES platform lib drivers chip interface)
foreach(LIBRARY ${SHARED_LIBRARIES})
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${LIBRARY}
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY})
endforeach()

###############################################################################
# Create shared library and link sub-shared libraries
###############################################################################
add_library(SHARED INTERFACE)
target_link_libraries(SHARED INTERFACE
    PLATFORM_LIB
    DRIVERS_LIB
    SHARED_LIB
    CHIP_LIB
    INTERFACE_LIB)

###############################################################################
# Link to project executables
###############################################################################
target_link_libraries(${PROJECT_BOOT_EXECUTABLE}
    SHARED)
target_link_libraries(${PROJECT_APP_EXECUTABLE}
    SHARED)