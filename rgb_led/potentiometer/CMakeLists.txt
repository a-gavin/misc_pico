
cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PICO_SDK_FREERTOS_SOURCE $ENV{FREE_RTOS_PATH}/FreeRTOS/Source)

project(potentiometer C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        -g
        )

add_library(freertos
        ${PICO_SDK_FREERTOS_SOURCE}/event_groups.c
        ${PICO_SDK_FREERTOS_SOURCE}/list.c
        ${PICO_SDK_FREERTOS_SOURCE}/queue.c
        ${PICO_SDK_FREERTOS_SOURCE}/stream_buffer.c
        ${PICO_SDK_FREERTOS_SOURCE}/tasks.c
        ${PICO_SDK_FREERTOS_SOURCE}/timers.c
        ${PICO_SDK_FREERTOS_SOURCE}/portable/MemMang/heap_3.c
        ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos PUBLIC
        .
        ${PICO_SDK_FREERTOS_SOURCE}/include
        ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0
)        

add_executable(potentiometer
        potentiometer.c
        )

# pull in common dependencies
target_link_libraries(potentiometer pico_stdlib hardware_pwm hardware_adc freertos)

# tell the pico library that you will be using usb serial and not an actual uart on the 
# processor
pico_enable_stdio_usb(potentiometer 1)
pico_enable_stdio_uart(potentiometer 0)


# create map/bin/hex file etc.
pico_add_extra_outputs(potentiometer)

add_custom_command(TARGET potentiometer POST_BUILD COMMAND arm-none-eabi-size potentiometer.elf)

# add url via pico_set_program_url
# example_auto_set_url(blink)
