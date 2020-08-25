import time
import board
import digitalio
import pulseio
from adafruit_debouncer import Debouncer

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

led = pulseio.PWMOut(board.P0_13, frequency=5000, duty_cycle=65535)

button_pin = digitalio.DigitalInOut(board.P0_11)
button_pin.direction = digitalio.Direction.INPUT
button_pin.pull = digitalio.Pull.UP
button = Debouncer(button_pin)

connected_led = digitalio.DigitalInOut(board.P0_14)
connected_led.direction = digitalio.Direction.OUTPUT

ble = BLERadio()
uart_service = UARTService()
advertisement = ProvideServicesAdvertisement(uart_service)

PI_SEND_INTERVAL = 1
PWM_INDICATE_BLINK_INTERVAL = 0.25
PWM_INDICATE_BLINKS = 5

# From https://rosettacode.org/wiki/Pi#Python
def calcPi():
    q, r, t, k, n, l = 1, 0, 1, 1, 3, 3
    while True:
        if 4*q+r-t < n*t:
            # Generators work in CircuitPython!  :-)  :-)  :-)
            yield n
            nr = 10*(r-n*t)
            n  = ((10*(3*q+r))//t)-10*n
            q  *= 10
            r  = nr
        else:
            nr = (2*q+r)*l
            nn = (q*(7*k)+2+(r*l))//(t*l)
            q  *= k
            t  *= l
            l  += 2
            k += 1
            n  = nn
            r  = nr

pi_digits = calcPi()

while True:
    connected_led.value = True
    ble.start_advertising(advertisement)
    while not ble.connected:
        # Wait for a connection.
        pass
    connected_led.value = False

    last_pi_send = time.monotonic()
    pct = 0
    pwm_on = True
    pi_gen_on = True
    led.duty_cycle = int((100 - pct) * 65535 / 100)

    pwm_indicate_time = time.monotonic()
    pwm_indicate_count = 0

    while ble.connected:
        if uart_service.in_waiting:
            raw_bytes = uart_service.read(uart_service.in_waiting)
            pct = int(raw_bytes.decode().strip())
            if pct < 0:
                pct = 0
            if pct > 100:
                pct = 100

            if pwm_on:
                led.duty_cycle = int((100 - pct) * 65535 / 100)

        if not pwm_on:
            if (time.monotonic() - pwm_indicate_time >
                PWM_INDICATE_BLINK_INTERVAL):
                pwm_indicate_time = time.monotonic()
                blink_pct = 0
                pwm_indicate_count += 1
                if pwm_indicate_count < PWM_INDICATE_BLINKS * 2:
                    if pwm_indicate_count % 2 == 1:
                        blink_pct = 100
                    led.duty_cycle = int((100 - blink_pct) * 65535 / 100)
                else:
                    pwm_on = True
                    led.duty_cycle = int((100 - pct) * 65535 / 100)

        if pi_gen_on and time.monotonic() - last_pi_send > PI_SEND_INTERVAL:
            text = "DIGIT = {}\r\n".format(next(pi_digits))
            uart_service.write(text.encode())
            last_pi_send = time.monotonic()

        button.update()
        if button.fell:
            pi_gen_on = not pi_gen_on
            pwm_on = False

            pwm_indicate_time = time.monotonic()
            pwm_indicate_count = 0
            led.duty_cycle = 65535
