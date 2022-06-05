import usb_cdc
import board
import busio
import digitalio
from time import sleep, monotonic

from dotstar import Dotstar
from safe_mode import SafeMode


class MainRelay():
    def __init__(
            self,
            pin,
            turn_on_delay = 2
            ):
        self.io = digitalio.DigitalInOut(pin)
        self.turn_on_delay = turn_on_delay
        
        self.io.direction = digitalio.Direction.OUTPUT
        self.io.value = False

    def turn_on(
            self,
            ):
        if not self.io.value:
            self.io.value = True
            sleep(self.turn_on_delay)

    def turn_off(
            self,
            ):
        self.io.value = False


# Setup for safe-mode.
safe = SafeMode(
        board.GP22,
        False,
        b'^safemode',
        )

# Setup to turn the power supply relay on and off.
main_relay = MainRelay(board.GP16)

# Serial over USB for control.
serial = usb_cdc.data
serial.timeout = 1

# Class for controlling the LEDs.
dotstar = Dotstar(
        busio.SPI(
            clock=board.GP18,
            MOSI=board.GP19,
            ),
        )

# String to hold serial data.
serial_string = b''

while True:
    # Reset to safe mode when chosen pin is shorted to ground.
    #safe.check_pin()

    # Read in from the USB serial.
    while True:
        serial_byte = serial.read()
        if serial_byte:
            if serial_byte == b'!':
                # Reset to safe mode if the command is sent.
                #safe.check_command(serial_byte)
                if serial_string == b'^poweroff':
                    # Turn relay off.
                    main_relay.turn_off()
                else:
                    # Need the relay to be on.
                    main_relay.turn_on()
                    # Write to the LEDs.
                    print(serial_string)
                    dotstar.send_all_frames(serial_string)
                    serial_string = b''
                break
            else:
                serial_string += serial_byte
        else:
            break
