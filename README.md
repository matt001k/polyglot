# polyglot
Open Source Embedded Bootloader Framework

`polyglot` offers a scalable,
secure way to load applications to embedded devices.
It uses an abstract "serial" layer
that allows for any type of serial connection to be used as a way to load firmware.
For example,
this serial line may be USB,
Bluetooth Connection,
TCP Socket, etc.
as long as it follows the requirements of the callback functions laid out in the bootloader's configuration.

`polyglot` utilzies AES-CBC encryption/decryption and SHA256 for signing firmware images.
These functionalities are all configurable in firmware through the the bootloader's configuration.

Below describes the list of commands available to send to the device.

![Bootloader Commands](firmware/docs/commands.svg)

The following describes the bootloader's flow when loading an application:

![Bootloader Flow](firmware/docs/bootloader_flow.svg)

## CMake Integration

In order to integrate the project with CMake the following must be included:

```
add_executable(${executable_name})
include(path/to/polyglot/firmware/polyglot.cmake)
setup_polyglot(
    ${path_to_ecdh_pem}
    ${executable_name}
)

...

add_subdirectory(${path/to/polyglot/firmware})
```

An executable must be added for the bootloader.
Then including the path to the `polyglot.cmake` file.
A function `setup_polyglot` is required with the following variables are needed:

- Path to the ECDH key used for signing the application
- Executable name of the bootloader

Lastly the subdirectory must be added for polyglot to build the library.
