# derived from the pico-examples provided by Raspberry Pi Foundation
#

add_executable(dotstar_strip
		dotstar_strip.c
        )

# pull in common dependencies
target_link_libraries(dotstar_strip pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(dotstar_strip 1)
pico_enable_stdio_uart(dotstar_strip 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(dotstar_strip)
