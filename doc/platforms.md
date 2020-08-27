+++
title = "sky blue trades | Nordic Tools Comparison: The Platforms"
template = "project_page.html"
+++

# Platforms

What are these platforms? Here's a quick list of what I'm going to
take a look at. Some of these are Nordic only, but most of them are
what I'm calling "supports everything" platforms, which are toolsets
that attempt to abstract a lot of things across different devices to
encourage code reuse and to make it easier for people to switch
between devices.

### nRF5 SDK with SEGGER Embedded Studio

This is the [toolchain recommended by Nordic][nrf5-sdk]. SEGGER make
their [Embedded Studio][ses] toolchain available for free for use with
Nordic devices.

### GCC + Makefiles

This is what I ended up using for the project I was working on.

### GCC + VS Code

One step up from my low rent approach. Lots of people seem to like [VS
Code][vs-code], and although IDEs aren't my cup of tea, it supports so
many plugins and approaches that it's worth looking at.

### PlatformIO

[A "supports everything" platform][platformio]. Well, not quite, but
it does support a lot of devices.

### ARM mbed

This is [ARM's "supports everything" option][mbed] (as long as it's
ARM). Nordic were part of the mbed project for a while, now they're
not, but the Nordic chips are still supported.

### Zephyr

The Linux of the microcontroller world? It's not Linux, though it's a
[project of the Linux Foundation][zephyr]. Aims to be a modern
operating system for microcontrollers. Another "supports everything"
option.

### CircuitPython

This is the first of two "ringer" platforms, using "languages that are
not C". [CircuitPython][circuitpython] has a reputation as something
of a toy, but having written some fairly significant things in Python,
I can't quite believe that. I've been told that it supports Bluetooth
on the nRF52840, so I want to see just how easy this stuff can be!

### Rust + RTFM

The other non-C choice. [Rust][rust] is pretty amazing, and I know
there's been a lot of activity getting it going on ARM processors.
There is some experimental BLE support for the nRF52840 floating
around. I don't think this will be a serious option for commercial
development, but I want to see what's possible as a proof of concept.



[nrf5-sdk]: https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK
[ses]: https://www.segger.com/products/development-tools/embedded-studio/
[vs-code]: https://code.visualstudio.com/
[platformio]: https://code.visualstudio.com/
[mbed]: https://os.mbed.com/
[zephyr]: https://www.zephyrproject.org/
[circuitpython]: https://circuitpython.org/
[rust]: https://www.rust-lang.org/
