# Pico SDK (setup includes TinyUSB)
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

# FreeRTOS
set(FREERTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel)
add_subdirectory(${FREERTOS_DIR})