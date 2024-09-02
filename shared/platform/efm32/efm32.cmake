###############################################################################
# Chip specific variables
###############################################################################
set(LINKER_FILE_BOOT
    ${CMAKE_SOURCE_DIR}/shared/platform/efm32/toolchain/efm32tg11b_boot.ld)
set(LINKER_FILE_APP
    ${CMAKE_SOURCE_DIR}/shared/platform/efm32/toolchain/efm32tg11b_app.ld)
target_link_options(${PROJECT_APP_EXECUTABLE} PRIVATE
    -Wl,-Map=${PROJECT_APP_NAME}.map,--cref;
    -T${LINKER_FILE_APP})
target_link_options(${PROJECT_BOOT_EXECUTABLE} PRIVATE
    -Wl,-Map=${PROJECT_BOOT_NAME}.map,--cref;
    -T${LINKER_FILE_BOOT})
