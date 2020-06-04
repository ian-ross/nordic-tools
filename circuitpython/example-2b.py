import time
import board
import pulseio

led = pulseio.PWMOut(board.P0_13, frequency=5000, duty_cycle=0)

duty_percent = 10

led.duty_cycle = int((100 - duty_percent) * 65535 / 100)

while True:
    pass
