+++
title = "sky blue trades | Nordic Tools Comparison: Summary"
template = "project_page.html"
+++

# Summary

So, what's the bottom line here? I don't think the development
platform recommended by Nordic for the nRF52840 (SEGGER Embedded
Studio with the nRF5 SDK) is usable for serious development. For sure,
you *can* use it, but you're making life significantly more difficult
for yourself than it needs to be. (For all the grubby details and the
evidence to back up this claim, you'll need to read through the
[SEGGER Embedded Studio + nRF5 SDK](nrf5-sdk-ses) notes.)

Most of the problems are with the nRF5 SDK. The SEGGER IDE is OK, as
these things go, if not great. But those SDK problems are really hard
to deal with. We need something better.

For what they're worth, here are my recommendations:


## Firmware development

For application development, use Zephyr, ideally via PlatformIO. It
really does seem to be the wave of the future, and peripheral and
platform coverage and documentation are only going to get better.

The combination of using the Linux devicetree system and Kconfig is
very good. It makes it very clear what hardware resources are exposed
via operating system APIs, and it makes it very easy to configure the
use of different subsystems and libraries. The difference between this
setup and the unholy mess that is the `sdk_config.h` file for the nRF5
SDK is really stark. Zephyr shows how these things *should* be done.

The kernel APIs in Zephyr are different from other platforms (they're
not POSIX, they're not like FreeRTOS or any other RTOS), but they're
easy to learn, and they're laid out quite logically. The APIs for more
complex subsystems (e.g. Bluetooth) also seem quite logical, and in
particular, the Bluetooth API has a nice combination of usability and
access to protocol details needed for more complex applications.

(You can see the details of the development of the example programs
using PlatformIO and Zephyr in the [PlatformIO +
Zephyr](platformio-zephyr) notes.)


## Board bring-up

For board bring-up, if your platform fulfils the requirements to get
CircuitPython going on it, do your EEs a favour and use it! It's
incredibly easy to use and it makes it extremely convenient to write
little test programs to exercise different bits of functionality.

(You can see the details of my experiments with CircuitPython in the
[CircuitPython](circuitpython) notes.)

That "fulfils the requirements" thing might require a little effort
though. You'll need a friendly programmer to set up a custom board
support package for your board and build a custom CircuitPython
distribution. This is particularly important for the nRF52840, since
more or less every pin is remappable.

I've not tried this, but it looks as though it ought not to be too
hard. For example, if you take a look at the [board definition for the
nRF52840 development
kit](https://github.com/adafruit/circuitpython/tree/main/ports/nrf/boards/pca10056),
it seems that more or less all you need to set up is the pinmux
definitions for your board, plus a bit of boilerplate. You do need to
have a board that supports USB, since CircuitPython relies on setting
the device up as a USB mass storage device. (The USB pins on the
nRF52840 aren't remappable, so they don't appear in the CircuitPython
pinmux definitions.) So you write the custom board definition, build a
custom CircuitPython distribution and install it onto your board.
(You'd probably need to flash the Nordic bootloader first to get the
necessary USB support going.)


## Final words

The nRF52840 is a good MCU, but the nRF5 SDK slightly spoils the
experience of using it. It's good to know that there are (much) better
options available: Zephyr for application development and
CircuitPython for experimentation and hardware development support
(that's what I'd use them for anyway).
