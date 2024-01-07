add_library(UTILITY STATIC
    terminal/terminal.cpp)

target_include_directories(UTILITY PUBLIC
    terminal
    .)

target_link_libraries(UTILITY PUBLIC BOOTLOADER ABSTRACTION)
target_link_libraries(${PROJECT_EXECUTABLE} UTILITY)