+++
title = "Nordic Semiconductor nRF52840 Tools Comparison"
[taxonomies]
categories = ["embedded"]
+++

I've been doing some work using the [Nordic Semiconductor nRF52840
chip][nrf52840] recently. It's a nice ARM Cortex-M4F SoC with a 2.4
GHz radio suitable for Bluetooth and IEEE 802.15.4 networking (things
like Thread and Zigbee). The code I've been writing has been using
[MQTT][mqtt], [OpenThread][ot] and [FreeRTOS][freertos], and the
experience of using the Nordic SDK has been a little disappointing.

There are a few different problems with the Nordic SDKs: mixtures of
legacy and new drivers with varying support for the different driver
versions across different higher-level libraries in the SDK; a very
painful configuration system (one configuration header with 1152
`#define`s in one version of the SDK I've been using); opaque binary
components for Bluetooth functionality; and so on.

So I want to find a better approach, or at least to satisfy myself
that the Nordic SDK really is the best option. As you'd expect, their
SDK *does* have very full coverage of device functionality, which
might be a determining factor. And once it's set up, it does work. But
modifying existing code to make use of a new device or library is
pretty nasty. There has to be a better way.

This project is an attempt to do a fair comparison of the options out
there. I've been inspired by Jay Carlson's magnum opus [The Amazing $1
Microcontroller][1-dollar], which did a superb job of comparing a huge
range of microcontrollers in what I think was a fair and objective
way. I'm planning to do about 1% of the amount of work that Jay did
for this, but I hope that will be enough to work out what tradeoffs
there might be in choosing different development platforms.


[nrf52840]: https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52840
[mqtt]: http://mqtt.org/
[ot]: https://openthread.io/
[freertos]: https://www.freertos.org/
[1-dollar]: https://jaycarlson.net/microcontrollers/
