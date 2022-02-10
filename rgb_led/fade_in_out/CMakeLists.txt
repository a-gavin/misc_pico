
cmake_minimum_required(VERSION 3.13)

message(STATUS "Include pico_sdk_import.cmake")
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

message(STATUS "Includes Complete")
message(STATUS "    gcc = ${CMAKE_C_COMPILER}")
message(STATUS "    g++ = ${CMAKE_CXX_COMPILER}")
message(STATUS "    as = ${CMAKE_ASM_COMPILER}")

project(fade_in_out C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
message(STATUS "Initialize the SDK")
pico_sdk_init()
message(STATUS "Initialize complete")

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't calfade_in_out
        -Wno-maybe-uninitialized
        -g
        )

add_executable(fade_in_out
        fade_in_out.c
        )

# pull in common dependencies
target_link_libraries(fade_in_out pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(fade_in_out)
