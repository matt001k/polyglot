set(PLATFORM_LIB PLATFORM)

# Building MbedTLS
execute_process(COMMAND
                python3 -m pip install --user -r ${CMAKE_CURRENT_LIST_DIR}/mbedtls/scripts/basic.requirements.txt)
option(ENABLE_TESTING OFF)
option(ENABLE_PROGRAMS OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/mbedtls)
target_compile_definitions(mbedcrypto PRIVATE MBEDTLS_CONFIG_FILE=<config/mbedtls_config.h>)
target_include_directories(mbedcrypto PRIVATE .)

add_library(SHARED_LIB INTERFACE)

target_include_directories(SHARED_LIB INTERFACE
    .
    CMSIS_5/CMSIS/Core/Include
    mbedtls/include)
target_link_libraries(SHARED_LIB INTERFACE mbedcrypto)
