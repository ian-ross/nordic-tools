When I first started looking at CircuitPython, I thought it was going
to be a bit of a toy. However, after a little playing with it, I'm
seriously impressed. The people at Adafruit have done a huge amount of
work to make things just work, and to make using Python on a
microcontroller a breeze (even for relatively complicated things, like
Bluetooth).

# Installation

## Mu editor

 - Doesn't work with Python 3.8...
 - So need to install Python 3.7 alongside 3.8.
 - Make a Python 3.7 virtual environment with `python3.7 -m venv
   ./.venv` and activate it (I used my usual stuff from the
   `make-virtual-env` script).
 - Upgrade PIP in the virtual environment: `pip install --upgrade
   pip`.
 - Install Mu: `pip install mu-editor`.

## Installing CircuitPython for nRF52840 dongle and dev kit

 - Download `.bin` install files.
 - Need to follow the "non-UF2" installation route.
 - Documentation is a bit lacking there...
 - Converting the `.bin` file to an Intel hex file (using `bin2hex.py`
   from `python-intelhex`) and flashing it with nRF Connect doesn't
   seem to work. The device doesn't appear as a USB drive as it's
   supposed to.
 - Let's try the dev kit instead of the dongle.
 - Here's a promising looking page:
   https://learn.adafruit.com/circuitpython-on-the-nrf52/nrf52840-bootloader
 - That seems to work! End up with the dev kit showing up as a USB
   drive called `NRF52BOOT` (on the nRF USB port, *not* the JLink
   one!).
 - Then download the latest CircuitPython UF2 file for the PCA10056
   board, copy it to the `NRF52BOOT` drive, and the thing reboots itself
   and reappears as a USB drive called `CIRCUITPY`. Success!

Conclusion: it's fair to say that the development kit is supported,
but not really the dongle. That's not CircuitPython's fault. It's more
down to Nordic preventing the bootloader on the dongle from being
overwritten by users, which makes installing CircuitPython a real
performance. The only reasonable sounding description I've seen
involves programming the dongle using the SWD pads exposed there,
using a JLink (or a bit-banged approximation of one using OpenOCD on a
Raspberry Pi). It doesn't seem to be possible via the dongle's USB
port. I'll definitely try the RPi + OpenOCD approach later, once I
want to have multiple devices, but it's a bit much.

## Installation statistics

The CircuitPython distribution itself appears small (the UF2 file for
the DK is 694K), but you have to remember that that's copied onto the
nRF52840 device.

There's also a ZIP file containing libraries, which is about 5.7 Mb
unzipped. This has a *very* large number of examples with it.

The bootloader stuff needed to set up the development kit for
installation is about 2.5 Gb, but you only need that once to set up
the board.

# Documentation

There's pretty good "getting started" documentation, which is
presumably a little easier to deal with if you have a board that's
really supported by CircuitPython.

Can't say what the reference documentation is like yet. There is some!

# Example 1: Blinky

Very simple, except for needing to fossick around in the board
definition to figure out the syntax used for the I/O pin definitions
(the Adafruit boards all have an LED on a pin called `D17`, but LED1
on the nRF52840 development kit is on `P0_13`).

# Example 2: PWM blinky

## Pre-defined example (`example-2a.py`)

Worked first time. Taken from the "CircuitPython Essentials" tutorial
(https://learn.adafruit.com/circuitpython-essentials/circuitpython-pwm)

## Add PWM to basic blinky (`example-2b.py`)

Nothing much to this: there's no configuration needed, the PWM library
is available without any drama.

# Example 3: BLE-controlled PWM blinky

## Pre-defined BLE example (`example-3a.py`)

Following something like this example:
https://learn.adafruit.com/bluetooth-light-switch-with-crickit-and-nrf52840/code-with-circuitpython

Need some libraries: these are just copied from the library bundle
into the `lib` directory on the `CIRCUITPY` drive. We need
`adafruit_ble` and `adafruit_bluefruit_connect`.

Oh wow. That was ridiculously easy. To get to an on/off LED controlled
by Bluetooth was really very easy. A bit of hacking things out of the
example mentioned above, and it worked more or less first time. The
*Bluefruit Connect* Android app is a handy little tool for playing
with these things.

## Add BLE to PWM blinky (`example-3b.py`)

1. Start from `example-2a.py`.
2. Add BLE imports based on `example-3a.py`.
3. Add "connected LED" and BLE UART service.
4. Add BLE connection handling based on `example-3a.py`.
5. Add UART packet processing: look in the
   `Adafruit_CircuitPython_Bundle` repository examples to see how to
   process raw UART packets.

Works more or less first time. (The only slight wrinkle is that the
GPIOs controlling LEDs are inverted: PWM = 0% is maximum brightness,
PWM = 100% is off.)

## Add PWM blinky to BLE example

Skipping this one because it's too easy.

# Example 4: Thread + MQTT equivalent of BLE PWM blinky

Oops. Thread isn't supported (latest discussion I found is here:
https://github.com/adafruit/circuitpython/issues/1162).

There is a CircuitPython MQTT library called MiniMQTT
(https://circuitpython.readthedocs.io/projects/minimqtt/en/latest/#)
but it relies on an underlying TCP transport. Without Thread support,
that's a non-starter on the nRF52840 (other boards supported by
CircuitPython have Wi-Fi capability, so MiniMQTT makes sense in that
context).

# Example 5: something "Almost Realistic" (`example-5.py`)

The problem here is that CircuitPython doesn't support any kind of
RTOS functionality in the Python code that you right. Modules
implemented in C can do whatever they like, of course, and some of the
Adafruit-supplied libraries run background tasks. For example, the
audio library can play samples while the main Python code does
something else. But you can't do that sort of thing in your Python
code.

The coding model for CircuitPython seems to be an active main loop:
`while True` check what's happened and respond to it. There's no
concept of threads or tasks, and no easy way to deal with asynchronous
events. That's not any kind of criticism, by the way: these things are
hard to deal with and forcing beginners to confront them head on would
be unfriendly, to say the least.

You *can* implement the required functionality for the "almost
realistic" example using a simple event loop and a state machine,
though I don't think you can do it in a low power "wait for event" way
(there's some discussion on this issue about low power sleep on the
nRF52840, which is not a simple thing to do, especially if you want to
be receptive to BLE messages while you're sleeping:
https://github.com/adafruit/circuitpython/issues/696).

This is done in the `example-5.py` code, just to show how it works.
It's pretty simple, but it's not really what this example is supposed
to be demonstrating!


# The judging criteria

## Installation

**How easy is it to install the platform?**

Very easy, apart from the nRF52840 dongle bootloader issue, which is a
problem for other platforms, not just CircuitPython.

**Is the download ridiculously large? How much disk space do you
need?**

By the standards of these things, pretty tiny.

**Does it work on Linux? Windows? MacOS? Any weird restrictions?**

Works everywhere with no problems, since you just need an editor and a
way to mount a USB drive.

**Is it free?**

Yes.

## Quick start

**How long is "Zero To Blinky"?**

30 seconds? Very quick.

**Are there enough examples?**

There are lots of examples, but they're not a easy to find as you
might hope. Most of the interesting ones seem to be in the Adafruit
CircuitPython bundle, which isn't that hard to find, but isn't
directly linked from the main `circuitpython.org` website.

**Does stuff just work?**

Yes. Quite amazingly so.

## Documentation

**Is there any?**

There is.

**Is there enough?**

Not quite sure about this. There are quite a few broken links around,
but I ended up just cloning the bundle repository and looking at the
examples there to see what was available and how to use it.

**Is it any good? (i.e. not just Doxygen...)**

It's mostly Sphinx (?) documentation generated from Python doc
comments. It's okay, and there are two series of "getting started"
articles on the Adafruit website that are much better. Those things
push the answer to this over to "yes".

**Are there tutorials?**

There are tutorials and examples. The tutorial matieral is all right,
and the examples are useful. There are also a lot of projects on the
Adafruit website, many of which contain interesting stuff.

## Basic workflow

### Edit

**Is there editor syntax support?**

It's Python.

**Do you have to use a specific IDE or can you use tools you're
 already familiar with?**

The Mu editor is recommended, since it knows about CircuitPython, but
you can use whatever you like, so long as it flushes writes to the USB
drive promptly.

**If you have to use a specific IDE, is it any good?**

You don't have to use anything in particular, but the recommended Mu
editor is fine, and a good thing for beginners.

### Compile

Most of this is irrelevant, since it's interpreted Python. I don't
know how it is writing CircuitPython libraries that have to do native
code things. I didn't look into that at all.

One thing that's pretty handy is how you set up libraries: just copy
them from the compiled CircuitPython bundle to the `lib` directory on
the USB drive.

### Flash

**Basically, does it work?**

Yes, because all you have to do is write files to a USB drive!

### Debug

This is also mostly irrelevant. There's no real debug support. There's
a serial console you can write to from your Python code, but that's
about it.


## Fancy workflows

None of this is relevant for CircuitPython. You can't do any of this
kind of thing, and the platform just isn't intended for the kind of
applications where you might be doing CI or automated testing.

## Functionality

### Coverage of device functionality

**What device peripherals have driver libraries?**

At least the following: GPIO, PWM, I2S, PDM, I2C, SPI, UART, SAADC, BLE.

A lot of the support code in `circuitpython/ports/nrf` looks familiar
from the nRF5 SDK.

**Are those libraries easy to use?**

Very. The core libraries are just there to use.

**Are there any options missing?**

Some of the weirder nRF52840 peripherals aren't supported (LDCOM,
QDEC, QSPI, for example). I don't have the first idea how to use those
though, so I don't really mind...

**If so, how easy is it to work around?**

It looks like it would be pretty easy to fork the main CircuitPython
repository to modify the nRF52840 support to get access to other
peripherals.

### Configuration

**How easy is it to use different libraries or drivers in your code?**

Really easy. Just copy them across to the `lib` directory on the
`CIRCUITPY` USB drive.

**Are there any configuration or code generation tools to help with
the setup?**

No, but no need.

### Libraries

**Are there higher-level libraries available for common functionality
 (e.g. communications, crypto, etc.)?**

Yes, quite a few helper libraries for Bluetooth stuff, plus others for
motor drivers, display handling, other communications things (e.g.
MQTT), and so on.

**How easy is it to incorporate third-party code into your projects?**

Easy: build an `mpy`, copy it into you `lib`.

### Frustration

**Basically: Did implementing the test programs make Ian angry?**

There was quite a lot of cursing as I tried to get CircuitPython
installed on the nRF52840 dongle. Life became much easier once I gave
up on that and just switched to using a development kit instead. I
only did that once I'd convinced myself that it would definitely
require connecting to the SWD pins on the dongle rather than
programming the thing through the USB connector and the bootloader. I
can understand why Nordic set these things up this way, making it
really hard to overwrite the bootloader, because doing so basically
bricks the dongle for its "normal" use, putting it into a state where
you need to program it via the SWD pins.

And there's a very nasty extra wrinkle there: when you erase the
device to program it, you zero out the `REGOUT0` internal register
that controls the GPIO reference voltage. Setting that to zero puts
the chip into a mode where it only understands 1.8V logic levels,
which doesn't work with the hardware on the dongle. The new setting
doesn't take effect until the chip is reset, so you *can* erase and
reprogram the thing, but if there's an interruption in power between
reset and reprogramming, you're left with a dongle that's totally
bricked. If you do that, you can only reprogram it via SWD using a
debugger that can do level shifting to the 1.8V levels. So don't do
that.

You can avoid all these problems either by using a development kit
(everything works swimmingly there), or using a system that doesn't
need to overwrite the Nordic bootloader. (If you're writing C, you
just set up the linker file to avoid the bootloader, and to put the
main program code starting at 0x1000 instead of the bottom of memory.
That doesn't seem to be a thing you can do with CircuitPython: you'd
have to build a special image for the dongle.)

Anyway, once I got going on the nRF52840 development kit, there was no
cursing, and I was very far from angry. When I first got a Bluetooth
example going, my reaction was more "Wow. That was easy." It's really
quite impressive. The nice people at Adafruit have done a *lot* of
work to make CircuitPython easy to use.
