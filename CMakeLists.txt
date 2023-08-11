cmake_minimum_required(VERSION 3.25)

#include(pico_sdk_import.cmake)

#project(IR-Positioning C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_executable(IR-Positioning
        main.c
        adc.c
        pwm.c
        fft_imp.c
        kiss_fft/kiss_fft.c
        )

target_include_directories(IR-Positioning PRIVATE
        .
        kiss_fft
    )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(IR-Positioning stdio pico_stdlib hardware_gpio hardware_spi hardware_pwm hardware_clocks kiss_fft )



# printf only via USB
pico_enable_stdio_uart(IR-Positioning 0)
pico_enable_stdio_usb(IR-Positioning 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(IR-Positioning)

# add url via pico_set_program_url
example_auto_set_url(IR-Positioning)