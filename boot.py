import usb_cdc
import storage
import supervisor

# Enable the second USB serial port (and console).
usb_cdc.enable(console=True, data=True)
# Enable the USB serial port (and disable console).
#usb_cdc.enable(console=False, data=True)

# Don't mount usb drive when booting.
# Note that there's no way to undo this unless you have a button and logic
# or console access to enter Safe Mode.
# Must also uncomment the `import storage` line at the top of this file.
storage.disable_usb_drive()

#Disable autoreload.
supervisor.disable_autoreload()
