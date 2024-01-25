###############################################################################
# ARM Specific Project Variables
###############################################################################
if(DEFINED ENV{AHRIMAN_ARM_TOOLCHAIN})
    message(STATUS "Using toolchain located at: " $ENV{AHRIMAN_ARM_TOOLCHAIN})
    set(TOOLCHAIN_LOCATION $ENV{AHRIMAN_ARM_TOOLCHAIN} CACHE INTERNAL "")
else()
    message(WARNING "Cannot find toolchain environment path.")
endif()

###############################################################################
# Compiler Settings
###############################################################################
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(COMPILER_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-gcc)
else()
    set(COMPILER_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-gcc.exe)
endif()

set(CMAKE_C_COMPILER ${COMPILER_NAME} CACHE PATH "" FORCE)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

###############################################################################
# Compiling Flags
###############################################################################
set(C_FLAGS "-mcpu=${MCU_USED} \
-mthumb \
-mfpu=${FP_UNIT_USED} \
-mfloat-abi=${FP_USED}\
${C_DEFS} \
-fdata-sections \
-ffunction-sections \
-Wall \
-O0 \
-g")


set(CMAKE_C_FLAGS ${C_FLAGS})
set(CMAKE_ASM_FLAGS ${C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU_USED} \
-mthumb \
-mfpu=${FP_UNIT_USED} \
-mfloat-abi=${FP_USED} \
-specs=nano.specs \
-specs=nosys.specs \
-lc \
-lm \
-lnosys \
-Wl,--gc-sections -g")
target_link_options(${PROJECT_APP_EXECUTABLE} PRIVATE
    -Wl,-Map=${PROJECT_APP_NAME}.map,--cref;
    -T${LINKER_FILE_APP})
target_link_options(${PROJECT_BOOT_EXECUTABLE} PRIVATE
    -Wl,-Map=${PROJECT_BOOT_NAME}.map,--cref;
    -T${LINKER_FILE_BOOT})
