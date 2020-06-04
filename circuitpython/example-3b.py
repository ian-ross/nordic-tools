import time
import board
import digitalio
import pulseio

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

led = pulseio.PWMOut(board.P0_13, frequency=5000, duty_cycle=65535)

connected_led = digitalio.DigitalInOut(board.P0_14)
connected_led.direction = digitalio.Direction.OUTPUT

ble = BLERadio()
uart_service = UARTService()
advertisement = ProvideServicesAdvertisement(uart_service)

SEND_INTERVAL = 5

while True:
    connected_led.value = True
    ble.start_advertising(advertisement)
    while not ble.connected:
        # Wait for a connection.
        pass
    connected_led.value = False

    last_send = time.monotonic()
    pct = 0
    led.duty_cycle = int((100 - pct) * 65535 / 100)

    while ble.connected:
        if uart_service.in_waiting:
            raw_bytes = uart_service.read(uart_service.in_waiting)
            pct = int(raw_bytes.decode().strip())
            if pct < 0:
                pct = 0
            if pct > 100:
                pct = 100
            led.duty_cycle = int((100 - pct) * 65535 / 100)
        if time.monotonic() - last_send > SEND_INTERVAL:
            text = "PWM = {}%\r\n".format(pct)
            uart_service.write(text.encode())
            last_send = time.monotonic()
