set(PLATFORM_LIB PLATFORM)

# Building MbedTLS
execute_process(COMMAND
                python3 -m pip install --user -r ${CMAKE_CURRENT_LIST_DIR}/mbedtls/scripts/basic.requirements.txt)
option(ENABLE_TESTING OFF)
option(ENABLE_PROGRAMS OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/mbedtls)
set(MBEDTLS_LIBS mbedcrypto mbedx509 mbedtls)
foreach(LIBS IN LISTS MBEDTLS_LIBS)
    target_compile_definitions(${LIBS} PRIVATE MBEDTLS_CONFIG_FILE=<config/mbedtls_config.h>)
    target_include_directories(${LIBS} PRIVATE .)
endforeach()

add_library(SHARED_LIB
    micro-ecc/uECC.c)

target_include_directories(SHARED_LIB PUBLIC
    .
    CMSIS_5/CMSIS/Core/Include
    micro-ecc
    mbedtls/include)
target_compile_options(SHARED_LIB PRIVATE -fomit-frame-pointer)
target_compile_definitions(SHARED_LIB PRIVATE uECC_OPTIMIZATION_LEVEL=3)
target_link_libraries(SHARED_LIB PRIVATE mbedcrypto)
