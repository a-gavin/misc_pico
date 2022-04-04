# Preconditions and project definitions
cmake_minimum_required(VERSION 3.13)

# Pull in SDK (Must be before project)
include(${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk/external/pico_sdk_import.cmake)

project(usb_printer C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
    -g
)

# Executable
add_executable(usb_printer)

# Library setup/configuration
#   Libraries "freertos_kernel" and "tinyusb" defined in sub-dir CMake files
set(CMAKE_BUILD_TYPE "Debug")
set(PICO_DEOPTIMIZED_DEBUG 1)
set(FREERTOS_PORT "GCC_ARM_CM0" CACHE STRING "Supported devices are only ARM CM0 at this time")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include CACHE STRING "FreeRTOS config file directory.")

add_subdirectory(src)
add_subdirectory(lib)

# Link libraries to executable
target_link_libraries(usb_printer freertos_kernel pico_stdlib)

# Configure USB serial and not an UART on the processor
pico_enable_stdio_usb(usb_printer 1)
pico_enable_stdio_uart(usb_printer 0)

# Create map/bin/hex file etc.
pico_add_extra_outputs(usb_printer)

# Printout executable info after build
add_custom_command(
    TARGET
        usb_printer
    POST_BUILD COMMAND
        arm-none-eabi-size usb_printer.elf
)