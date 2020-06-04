import time

import board
import digitalio

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.button_packet import ButtonPacket

led = digitalio.DigitalInOut(board.P0_13)
led.direction = digitalio.Direction.OUTPUT

connected_led = digitalio.DigitalInOut(board.P0_14)
connected_led.direction = digitalio.Direction.OUTPUT

ble = BLERadio()
uart_service = UARTService()
advertisement = ProvideServicesAdvertisement(uart_service)

# LED values are inverted.
on = True
led.value = True

print("BLE Light Switch")
print("Use Adafruit Bluefruit app to connect")
while True:
    connected_led.value = True
    ble.start_advertising(advertisement)
    while not ble.connected:
        # Wait for a connection.
        pass
    connected_led.value = False

    while ble.connected:
        if uart_service.in_waiting:
            # Packet is arriving.
            packet = Packet.from_stream(uart_service)
            if isinstance(packet, ButtonPacket):
               if packet.pressed and packet.button == ButtonPacket.BUTTON_1:
                led.value = not led.value
