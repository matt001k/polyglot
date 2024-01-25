add_library(CHIP_LIB STATIC
    GD25Q80C/GD25Q80C.c
    NAU88C10/NAU88C10.c)

target_include_directories(CHIP_LIB PUBLIC
    GD25Q80C
    NAU88C10)

target_link_libraries(CHIP_LIB PUBLIC
    PLATFORM_LIB)
