add_executable(dotstar_strip
		dotstar_strip.c
        )

# pull in common dependencies
target_link_libraries(dotstar_strip pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(dotstar_strip)
