add_library(INTERFACE_LIB STATIC
    bootloader/bootloader.c
    ext_flash/ext_flash.c
    partition/partition.c
    time/time.c)

target_include_directories(INTERFACE_LIB PUBLIC
    bootloader
    ext_flash
    partition
    time)

target_link_libraries(INTERFACE_LIB PUBLIC
    PLATFORM_LIB
    CHIP_LIB)