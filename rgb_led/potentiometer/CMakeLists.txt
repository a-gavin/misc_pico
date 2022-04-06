message(STATUS "Configure potentiometer")
add_library(freertos_potentiometer
    ${FREERTOS_KERNEL_SOURCE}/event_groups.c
    ${FREERTOS_KERNEL_SOURCE}/list.c
    ${FREERTOS_KERNEL_SOURCE}/queue.c
    ${FREERTOS_KERNEL_SOURCE}/stream_buffer.c
    ${FREERTOS_KERNEL_SOURCE}/tasks.c
    ${FREERTOS_KERNEL_SOURCE}/timers.c
    ${FREERTOS_KERNEL_SOURCE}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos_potentiometer PUBLIC
    .
    ${FREERTOS_KERNEL_SOURCE}/include
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0
)        

add_executable(potentiometer
    potentiometer.c
)

# pull in common dependencies
target_link_libraries(potentiometer pico_stdlib hardware_pwm hardware_adc freertos_potentiometer)

# tell the pico library that you will be using usb serial and not an actual uart on the 
# processor
pico_enable_stdio_usb(potentiometer 1)
pico_enable_stdio_uart(potentiometer 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(potentiometer)
message(STATUS "End configure potentiometer")