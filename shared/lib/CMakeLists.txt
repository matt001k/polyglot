set(PLATFORM_LIB PLATFORM)

add_library(SHARED_LIB INTERFACE)

target_include_directories(SHARED_LIB INTERFACE
    CMSIS_5/CMSIS/Core/Include)

#target_link_libraries(SHARED_LIB)