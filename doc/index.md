+++
title = "sky blue trades | Nordic Semiconductor nRF52840 Tools Comparison"
template = "project_top_level.html"
+++

# Nordic Semiconductor nRF52840 Tools Comparison

**Warning: this needs a massive TL;DR warning. It got really out of
hand. Unless you're a true glutton for punishment, you probably want
to skip straight to the [SUMMARY](summary).**

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

# The approach

So, the idea here is to take a bunch of development platforms
(toolchains, SDKs, IDEs, languages, however you define a "development
platform"), and to compare them for usability on the nRF52840 by
developing a small set of example programs. I came up with some more
or less objective criteria to judge these things on.

 - [Example programs](examples): I chose a small series of example
   programs to implement on each platform, increasing in complexity
   from "Blinky" to something involving Bluetooth, some GPIO stuff,
   some timers and an RTOS. These aim to be realistic without being
   too large. I also want to exercise the important task of adding
   functionality to existing code (e.g. add Bluetooth to the code in
   your widget, add access to a PWM timer/library, that sort of
   thing), because this is something where a bad configuration system
   really hurts you, and it's one of the biggest pain points I had
   with the SDK that Nordic recommends.

 - [Platforms](platforms): I picked a few different development
   environments and SDKs to test, based on what I'd already been
   using, a few things I'd seen that looked interesting, a couple of
   suggestions from Chris Gammell, and one total ringer to be dealt
   with at some future date (Rust: probably not practical for "normal"
   development yet, but I'm interested to see what you can do with
   it).

 - [Assessment criteria](criteria): This is more or less just a
   checklist of things to think about. Is the platform easy to
   install? Are there examples? Is there documentation? Is it any
   good? Is it easy to build and flash the example code? How easy is
   the IDE (if any) to use? How easy is it to get things done with the
   SDK? Some fancier stuff, and (maybe most importantly) just how
   frustrated did I get trying to write the example programs using the
   platform? (I figure that if, starting from zero with a platform, I
   only experience 2 or 3/10 on a frustration scale doing things, it's
   really rather good. The experience of blowing the top off the
   frustration meter is unfortunately more common...)

I wasn't too hard on myself about sticking to the programme. If a
platform was giving me a really hard time, I just bailed early from
the series of example programs. If it takes superhuman efforts to make
an LED blink, it's probably not going to be a bundle of fun getting
something with Bluetooth and an RTOS going, and it's going to be even
less fun using the thing for real work. I also didn't take too much
care over code quality. Most of the example programs are cut-and-paste
cargo cult programming, done in a slapdash kind of way just to see how
easy it is to get things working. If you can get stuff going in this
casual way, then things are likely to be pretty good when you spend a
bit more time and care.

# The evaluations

Some of these are really long. None of this is particularly simple,
and there are lots of things to look at, so that's unavoidable, but
does make these things kind of boring. Seriously, the
[summary](summary) is pretty great. Short and sweet. Smooth and
succinct. I won't be offended if you skip right there!

 - [SEGGER Embedded Studio + nRF5 SDK](nrf5-sdk-ses) (recommended by Nordic)
 - [GCC + Makefiles + nRF5 SDK (and similar)](gcc-makefiles)
 - [PlatformIO + Zephyr](platformio-zephyr)
 - [PlatformIO + ARM mbed](platformio-mbed)
 - [CircuitPython](circuitpython)
 - Rust + RTFM + various... (TBD)

# The summary

[Conclusions and summary on a separate page](summary) so I can point
people at it without making them read all this stuff...

[nrf52840]: https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52840
[mqtt]: http://mqtt.org/
[ot]: https://openthread.io/
[freertos]: https://www.freertos.org/
[1-dollar]: https://jaycarlson.net/microcontrollers/
