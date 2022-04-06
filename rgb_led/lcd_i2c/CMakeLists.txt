message(STATUS "Configure lcd_i2c")
add_library(freertos_lcd_i2c
    ${FREERTOS_KERNEL_SOURCE}/event_groups.c
    ${FREERTOS_KERNEL_SOURCE}/list.c
    ${FREERTOS_KERNEL_SOURCE}/queue.c
    ${FREERTOS_KERNEL_SOURCE}/stream_buffer.c
    ${FREERTOS_KERNEL_SOURCE}/tasks.c
    ${FREERTOS_KERNEL_SOURCE}/timers.c
    ${FREERTOS_KERNEL_SOURCE}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos_lcd_i2c PUBLIC
    .
    ${FREERTOS_KERNEL_SOURCE}/include
    ${FREERTOS_KERNEL_SOURCE}/portable/GCC/ARM_CM0
)        

add_executable(lcd_i2c
    lcd_i2c.c
    constants.h
)

# pull in common dependencies
target_link_libraries(lcd_i2c pico_stdlib hardware_i2c freertos_lcd_i2c)

# tell the pico library that you will be using usb serial and not an actual uart on the 
# processor
pico_enable_stdio_usb(lcd_i2c 1)
pico_enable_stdio_uart(lcd_i2c 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(lcd_i2c)
message(STATUS "End configure lcd_i2c")