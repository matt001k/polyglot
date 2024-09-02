##################################################################
# Print executable size
##################################################################
add_custom_command(TARGET ${PROJECT_BOOT_EXECUTABLE}
        POST_BUILD
        COMMAND ${SIZE} ${PROJECT_BOOT_EXECUTABLE})
add_custom_command(TARGET ${PROJECT_APP_EXECUTABLE}
        POST_BUILD
        COMMAND ${SIZE} ${PROJECT_APP_EXECUTABLE})

##################################################################
# Create binary/hex file
##################################################################
add_custom_command(TARGET ${PROJECT_BOOT_EXECUTABLE}
        POST_BUILD
        COMMAND ${OBJCOPY} -O ihex ${PROJECT_BOOT_EXECUTABLE} ${PROJECT_BOOT_NAME}.hex
        COMMAND ${OBJCOPY} -O binary ${PROJECT_BOOT_EXECUTABLE} ${PROJECT_BOOT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_APP_EXECUTABLE}
        POST_BUILD
        COMMAND ${OBJCOPY} -O ihex ${PROJECT_APP_EXECUTABLE} ${PROJECT_APP_NAME}.hex
        COMMAND ${OBJCOPY} -O binary ${PROJECT_APP_EXECUTABLE} ${PROJECT_APP_NAME}.bin)
