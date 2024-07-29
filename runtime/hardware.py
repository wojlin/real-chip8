import platform
from gpiozero import DigitalOutputDevice, DigitalInputDevice, LED
from gpiozero.pins.mock import MockFactory
from gpiozero import Device
from time import sleep

# Check if running on a Raspberry Pi or on another platform
if platform.system() == "Linux" and "raspberrypi" in platform.uname().machine:
    # Running on Raspberry Pi
    Device.pin_factory = None  # Real GPIO
else:
    # Running on PC, use mock GPIO
    Device.pin_factory = MockFactory()


class Keypad:
    def __init__(self):
        # Define the GPIO pins using BCM numbering for rows and columns
        self.row_pins = [5, 6, 13, 19]  # BCM pin numbers for rows
        self.col_pins = [26, 16, 20, 21]  # BCM pin numbers for columns

        # Define your key layout
        self.keys = [
            ['1', '2', '3', 'C'],
            ['4', '5', '6', 'D'],
            ['7', '8', '9', 'E'],
            ['A', '0', 'B', 'F']
        ]

        # Initialize row and column pins
        self.row_devices = [DigitalOutputDevice(pin) for pin in self.row_pins]
        self.col_devices = [DigitalInputDevice(pin, pull_up=True) for pin in self.col_pins]

    def scan(self):
        # Scan the keypad to find which key is pressed
        for row_num, row in enumerate(self.row_devices):
            row.on()  # Set the current row high
            for col_num, col in enumerate(self.col_devices):
                if col.value == 0:  # Check if the current column is low (key pressed)
                    row.off()
                    return self.keys[row_num][col_num]
            row.off()  # Set the current row low again
        return None  # No key pressed

    def detect_key(self):
        while True:
            key = self.scan()
            if key:
                print(f"Key pressed: {key}")
                return key


class Leds:
    def __init__(self):
        # Initialize LED pins
        self.yellow_pin = LED(2)
        self.green_pin = LED(3)

    def turn_yellow(self, state: bool):
        if state:
            self.yellow_pin.on()
        else:
            self.yellow_pin.off()

    def turn_green(self, state: bool):
        if state:
            self.green_pin.on()
        else:
            self.green_pin.off()