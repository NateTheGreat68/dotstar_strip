from adafruit_bus_device.spi_device import SPIDevice
class Dotstar():
    def __init__(
            self,
            spi,
            ):
        self._spi_device = SPIDevice(spi)
        self._locked = False
        self._led_count = 0

    def send_all_frames(
            self,
            led_frames,
            ):
        self.send_start_frame()
        self.send_led_frames(led_frames)
        self.send_end_frame()

    def send_start_frame(
            self,
            ):
        with self._spi_device as spi:
            spi.write(b'\x00'*4)

    def send_led_frames(
            self,
            led_frames,
            ):
        with self._spi_device as spi:
            spi.write(b'\xff\xff\xff\xff'*67)
            #TODO

    def send_end_frame(
            self,
            ):
        # Ceiling(led_count / 2) + 4
        end_bytes = -(self._led_count // -2) + 4 # ceil(led_count/2)+4

        with self._spi_device as spi:
            spi.write(b'\x00'*end_bytes)
