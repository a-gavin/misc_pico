cmake_minimum_required(VERSION 3.13)

set(PICO_SDK ${CMAKE_SOURCE_DIR}/lib/pico-sdk)
set(FREERTOS_KERNEL_SOURCE ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel)

message(STATUS "Include pico_sdk_import.cmake")
include(${PICO_SDK}/external/pico_sdk_import.cmake)

message(STATUS "Includes Complete")
message(STATUS "    gcc = ${CMAKE_C_COMPILER}")
message(STATUS "    g++ = ${CMAKE_CXX_COMPILER}")
message(STATUS "    as = ${CMAKE_ASM_COMPILER}")

project(misc_pico_projects C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
message(STATUS "Initialize Pico SDK")
pico_sdk_init()
message(STATUS "Initialize Pico SDK complete")

message(STATUS "Configure project-wide libraries")
add_compile_options(
    -Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't calfade_in_out
    -Wno-maybe-uninitialized
    -g
)

# Library setup/configuration
set(CMAKE_BUILD_TYPE "Debug")
set(PICO_DEOPTIMIZED_DEBUG 1)

# FreeRTOS library created separately for each executable as each may have different configurations
set(FREERTOS_PORT "GCC_ARM_CM0" CACHE STRING "Supported devices are only ARM CM0 at this time")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include CACHE STRING "FreeRTOS config file directory.")

add_subdirectory(rgb_led)
add_subdirectory(usb)