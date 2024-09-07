set(PEM_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/_tmp/)
set(PEM_HEADER_FILE ${PEM_HEADER_DIR}/ecdh_key.h)

add_custom_command(
    OUTPUT ${PEM_HEADER_FILE}
    COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/../scripts/key_translate.py ${POLYGLOT_PUBLIC_PEM} ${PEM_HEADER_DIR}
)

add_custom_target(
    polyglot_prepare ALL
    DEPENDS ${PEM_HEADER_FILE}
)

add_dependencies(
    ${POLYGLOT_EXECUTABLE}
    polyglot_prepare
)

add_library(BOOTLOADER STATIC
    abstraction/aes/aes.c
    abstraction/ecc/ecc.c
    abstraction/hold/hold.c
    abstraction/init/init.c
    abstraction/jump/jump.c
    abstraction/led/led.c
    abstraction/nvm/nvm.c
    abstraction/serial/serial.c
    abstraction/sha/sha256.c
    abstraction/systick/systick.c
    abstraction/wdt/wdt.c
    interface/buffer/buffer.c
    interface/command/command.c
    interface/data/data.c
    interface/loader/loader.c
    interface/table/table.c
    interface/writer/writer.c
    lib/crc/crc32.c
    lib/schedule/schedule.c
    lib/helper/helper.c
    main/run/run.c
    task/blink/blink.c
    task/update/update.c
    task/timeout/timeout.c
)

target_include_directories(BOOTLOADER PUBLIC
    ../
    abstraction/aes
    abstraction/ecc
    abstraction/hold
    abstraction/init
    abstraction/jump
    abstraction/led
    abstraction/nvm
    abstraction/serial
    abstraction/sha
    abstraction/systick
    abstraction/wdt
    interface/buffer
    interface/command
    interface/data
    interface/loader
    interface/table
    interface/validator
    interface/writer
    lib/crc
    lib/dict
    lib/helper
    lib/schedule
    main/run
    task/blink
    task/update
    task/timeout
    ${PEM_HEADER_DIR}
)

target_sources(${POLYGLOT_EXECUTABLE} PRIVATE main/main.c)
target_link_libraries(${POLYGLOT_EXECUTABLE}
    BOOTLOADER)

add_custom_command(
    TARGET BOOTLOADER
    POST_BUILD
    COMMAND cmake -E remove ${PEM_HEADER_FILE}
)
