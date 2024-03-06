add_library(BOOTLOADER STATIC
    abstraction/aes/aes.c
    abstraction/verify/verify.c
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
    interface/validator/validator.c
    lib/crc/crc32.c
    lib/schedule/schedule.c
    lib/helper/helper.c
    main/run/run.c
    task/blink/blink.c
    task/update/update.c
    task/timeout/timeout.c)

target_include_directories(BOOTLOADER PUBLIC
    ../
    abstraction/aes
    abstraction/verify
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
    lib/crc
    lib/dict
    lib/helper
    lib/schedule
    main/run
    task/blink
    task/update
    task/timeout)

target_link_libraries(BOOTLOADER PUBLIC
    SHARED)

target_sources(${PROJECT_BOOT_EXECUTABLE} PRIVATE main/main.c)
target_link_libraries(${PROJECT_BOOT_EXECUTABLE}
    BOOTLOADER)
