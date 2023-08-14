cmake_minimum_required(VERSION 3.25)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


#include(pico_sdk_import.cmake)

#project(IR-Positioning C CXX ASM)
pico_sdk_init()

add_executable(IR-Positioning
        main.c
        pwm.c
        adc.c
        fft_imp.c
        kiss_fft/kiss_fft.c
)

target_include_directories(IR-Positioning PRIVATE
        .
        kiss_fft
)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(IR-Positioning
        pico_stdlib
        hardware_gpio
        hardware_spi
        hardware_pwm
        hardware_clocks
        hardware_timer
)


# Enable a summary of used memory at the end of linking process
# Some compiler versions do not understand --no-warm-rwx-segments
#target_link_options(IR-Positioning PUBLIC "-Wl,--print-memory-usage,--no-warn-rwx-segments")
#add_custom_command(TARGET IR-Positioning
#    POST_BUILD
#    COMMENT "Code size:"
#    COMMAND arm-none-eabi-size IR-Positioning.elf
#    VERBATIM
#)
# printf only via USB
pico_enable_stdio_uart(IR-Positioning 1)
pico_enable_stdio_usb(IR-Positioning 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(IR-Positioning)
