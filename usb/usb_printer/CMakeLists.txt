message(STATUS "Configure usb_printer")
add_library(freertos_usb_printer
    ${FREERTOS_KERNEL_SOURCE}/event_groups.c
    ${FREERTOS_KERNEL_SOURCE}/list.c
    ${FREERTOS_KERNEL_SOURCE}/queue.c
    ${FREERTOS_KERNEL_SOURCE}/stream_buffer.c
    ${FREERTOS_KERNEL_SOURCE}/tasks.c
    ${FREERTOS_KERNEL_SOURCE}/timers.c
    ${FREERTOS_KERNEL_SOURCE}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos_usb_printer PUBLIC
    ${FREERTOS_KERNEL_SOURCE}/include
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0
)        

add_executable(usb_printer)

add_subdirectory(src)

# Link libraries to executable
target_link_libraries(usb_printer
    freertos_usb_printer
    pico_stdlib pico_unique_id pico_bootsel_via_double_reset
    tinyusb_device tinyusb_board tinyusb_additions  # Created in Pico SDK build system
)

pico_enable_stdio_uart(usb_printer 1)
pico_enable_stdio_semihosting(usb_printer 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(usb_printer)