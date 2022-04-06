message(STATUS "Configure fade_in_out")
add_executable(fade_in_out
    led_pwm.c
)

# pull in common dependencies
target_link_libraries(fade_in_out pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(fade_in_out)
message(STATUS "Configure fade_in_out complete")