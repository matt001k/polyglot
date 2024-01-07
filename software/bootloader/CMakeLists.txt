add_library(BOOTLOADER STATIC
    interface/command/command.cpp
    interface/data/data.cpp
    interface/serial/serial.cpp
    lib/crc/crc32.cpp)

target_include_directories(BOOTLOADER PUBLIC
    interface/command
    interface/data
    interface/serial
    lib/crc
    lib/dict
    utility)

target_link_libraries(BOOTLOADER PUBLIC)