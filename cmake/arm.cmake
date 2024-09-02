###############################################################################
# Compiler Settings
###############################################################################
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(COMPILER_NAME ${COMPILER_PATH}arm-none-eabi-gcc)
else()
    set(COMPILER_NAME ${COMPILER_PATH}arm-none-eabi-gcc.exe)
endif()

set(CMAKE_C_COMPILER    ${COMPILER_PATH}arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER  ${COMPILER_PATH}arm-none-eabi-g++)
set(AS                  ${COMPILER_PATH}arm-none-eabi-as)
set(CMAKE_AR            ${COMPILER_PATH}arm-none-eabi-gcc-ar)
set(OBJCOPY             ${COMPILER_PATH}arm-none-eabi-objcopy)
set(OBJDUMP             ${COMPILER_PATH}arm-none-eabi-objdump)
set(SIZE                ${COMPILER_PATH}arm-none-eabi-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

###############################################################################
# Compiling Flags
###############################################################################
set(MCU_USED "cortex-m0plus")
set(FP_USED "soft")
set(FP_UNIT_USED "auto")

set(C_FLAGS "-mcpu=${MCU_USED} \
-mthumb \
-mfpu=${FP_UNIT_USED} \
-mfloat-abi=${FP_USED}\
${C_DEFS} \
-fdata-sections \
-ffunction-sections \
-Wall \
-Og \
-g")


set(CMAKE_C_FLAGS ${C_FLAGS})
set(CMAKE_ASM_FLAGS ${C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU_USED} \
-mthumb \
-mfpu=${FP_UNIT_USED} \
-mfloat-abi=${FP_USED} \
-specs=nano.specs \
-lc \
-lm \
-Wl,--gc-sections -g")
