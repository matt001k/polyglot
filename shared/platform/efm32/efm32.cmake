###############################################################################
# Chip specific variables
###############################################################################
set(MCU_USED "cortex-m0plus")
set(FP_USED "soft")
set(FP_UNIT_USED "auto")
set(LINKER_FILE_BOOT
    ${CMAKE_SOURCE_DIR}/shared/platform/efm32/toolchain/efm32tg11b_boot.ld)
set(LINKER_FILE_APP
    ${CMAKE_SOURCE_DIR}/shared/platform/efm32/toolchain/efm32tg11b_app.ld)

###############################################################################
# ARM Specific CMAKE configuration
###############################################################################
include(${CMAKE_SOURCE_DIR}/cmake/arm.cmake)