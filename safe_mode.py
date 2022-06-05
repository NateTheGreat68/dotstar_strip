import digitalio
import microcontroller

class SafeMode():
    def __init__(
            self,
            pin,
            pin_reset_value,
            command = None,
            ):
        self.io = digitalio.DigitalInOut(pin)
        self.pin_reset_value = pin_reset_value
        self.command = command

        self.io.direction = digitalio.Direction.INPUT
        if self.pin_reset_value: # == True
            self.io.pull = digitalio.Pull.DOWN
        else: # == False
            self.io.pull = digitalio.Pull.UP

    @staticmethod
    def reset_to_safe_mode(
            ):
        microcontroller.on_next_reset(microcontroller.RunMode.SAFE_MODE)
        microcontroller.reset()

    def check_pin(
            self,
            ):
        if self.io.value == self.pin_reset_value:
            SafeMode.reset_to_safe_mode()

    def check_command(
            self,
            command,
            ):
        if self.command \
                and command == self.command:
            SafeMode.reset_to_safe_mode()
