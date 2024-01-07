add_library(ABSTRACTION STATIC
    bootloader/bootloader.cpp)

target_include_directories(ABSTRACTION PUBLIC
    bootloader)

target_link_libraries(ABSTRACTION PUBLIC BOOTLOADER CP2110 UTILITY)
target_link_libraries(${PROJECT_EXECUTABLE} ABSTRACTION)