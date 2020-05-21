+++
title = "The Examples"
[taxonomies]
categories = ["embedded"]
+++

# The example programs

For each platform, the goal is to implement five test programs, of
increasing complexity, starting where possible from pre-defined
examples. (Because everyone starts by copying examples. Starting from
a blank page/empty project directory triggers too much existential
angst.) None of these programs are complicated. They should be *easy*
to write. But I know that they probably won't be, because everything
is more complicated than you expect. It's quite possible that I'll
only get so far with some of these and will abandon if things get too
hard on any particular platform.

As I go through developing and testing these things for each platform,
I'll keep track of the highs and lows of using the platform, using a
set of criteria listed in the next section.

#### Example 1: Blinky

The nRF52840 development kit has LEDs. Let's make them blink.

Functionality exercised:

 - Installation and setup of platform.
 - Setting up a new project.
 - Configure, build, flash.
 - Default clock and power setup.
 - GPIOs.
 - Simple delays in single-threaded code.

This one really should just be a case of picking an existing example,
building and flashing it.

#### Example 2: PWM blinky with programmable duty cycle

PWM dimming of that LED. The nRF52840 has the usual sort of PWM
timers.

Here we're going to introduce a methodology I'm going to apply for all
of these more complex examples, which is to take more than one route
to get to the final code. Here, there are two things to look at:

1. Use a pre-defined PWM example: this will give a clear demonstration
   that the thing we're trying to do is possible.
2. Add PWM dimming to the basic blinky form Example 1: the idea here
   is to *extend* an existing code to use new functionality.

Functionality exercised:

 - Maybe more on clock setup.
 - Timer setup for PWM.

There's not that much new functionality in this example, but it's
intended to explore how easy it is to add new functionality based on
documentation and examples provided. This is one of the things that
proved most difficult using the Nordic nRF5 SDK, and it seems like a
fairly critical thing to be able to do.

#### Example 3: BLE-enabled PWM blinky

Now we're getting to the fun stuff. Control a PWM dimmed LED via
Bluetooth. We'll use Nordic's [nRF Connect for
Mobile][nrf-connect-mobile] Android application to talk to code
running on the nRF52840 development kit.

There are two ways to go at this:

1. Add BLE to the PWM blinky from Example 2.
2. Add PWM blinky functionality to a pre-defined BLE example.

Functionality exercised:

 - Bluetooth.
 - Maybe device conflicts: I'll see if I can organise things so that
   the PWM blinky uses a timer that the Bluetooth library uses to see
   if the toolset warns me about that.

I'll also check out a pre-defined BLE example, since Bluetooth setup
is kind of complicated and it would be interesting to see how the
different toolsets handle this.

#### Example 4: MQTT + Thread-enabled PWM blinky

The 2.4 GHz radio in the nRF52840 is good for more than Bluetooth.
Let's switch Example 4 over to use MQTT over Thread instead of
Bluetooth.

What I really want to try here is just taking the Example 3 code and
switching directly over from Bluetooth to Thread + MQTT without any
messing around starting from the ground up using pre-defined examples.
This might get ugly, so I'll be prepared to fall back on pre-defined
examples to get things going.

Functionality exercised:

 - A different network stack (Thread).
 - More complex application-level library (MQTT).
 - More complicated switching in and out of software components.

I'll use an OpenThread Network Coprocessor (NCP) running on an
nRF52840 dongle to provide a gateway from a Linux machine to the
OpenThread network. The Linux machine will run the MQTT broker, and
we'll use some little command line utilities to talk to nRF52840
devices running the PWM dimmer code.

#### Example 5: Something Almost Realistic

The idea here is to have a little application using an RTOS to do a
few different things at once, some of them involving Bluetooth
communications, but also including a bit of computation and a bit of
external I/O. This is about the minimum useful set of functionality I
can imagine for any kind of IoT or similar device.

What this thing will do:

 - PWM blinky with BLE remote control (so building on Example 3);
 - Generate digits of &pi; in a separate RTOS task and publish them
   via BLE;
 - Use a GPIO button to switch digit generation on and off (with
   temporary timed interruption of the PWM blinky as "UI feedback").

Functionality exercised:

 - RTOS.
 - Asynchronous GPIO input.
 - Multi-threaded access to BLE communications.


[nrf-connect-mobile]: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Connect-for-mobile
