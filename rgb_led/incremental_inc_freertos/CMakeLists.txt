message(STATUS "Configure incremental_inc_freertos")
add_library(freertos_incremental_inc
    ${FREERTOS_KERNEL_SOURCE}/event_groups.c
    ${FREERTOS_KERNEL_SOURCE}/list.c
    ${FREERTOS_KERNEL_SOURCE}/queue.c
    ${FREERTOS_KERNEL_SOURCE}/stream_buffer.c
    ${FREERTOS_KERNEL_SOURCE}/tasks.c
    ${FREERTOS_KERNEL_SOURCE}/timers.c
    ${FREERTOS_KERNEL_SOURCE}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos_incremental_inc PUBLIC
    .
    ${FREERTOS_KERNEL_SOURCE}/include
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0
)        

add_executable(incremental_inc_freertos
    incremental_inc_freertos.c
)

# pull in common dependencies
target_link_libraries(incremental_inc_freertos pico_stdlib hardware_pwm freertos_incremental_inc)

# tell the pico library that you will be using usb serial and not an actual uart on the 
# processor
pico_enable_stdio_usb(incremental_inc_freertos 1)
pico_enable_stdio_uart(incremental_inc_freertos 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(incremental_inc_freertos)
message(STATUS "End configure incremental_inc_freertos")