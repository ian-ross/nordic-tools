# Installation

If you start at the [main nRF52840
page](https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52840)
on the Nordic Semi website, after a bit of poking around, you get to
the [IDEs and toolchains
page](https://www.nordicsemi.com/Software-and-tools/Development-Tools/IDEs-and-Toolchains)
where the SEGGER Embedded Studio is (sort of) recommended for use.

[SEGGER Embedded
Studio](https://www.segger.com/products/development-tools/embedded-studio/)
(SES) is a commercial product that's free for use with Nordic Semi
devices (see the section headed "Free Commercial Use License" [here](
https://www.segger.com/products/development-tools/embedded-studio/license/licensing-conditions/)).
During installation it's not immediately obvious that this kind of
licensing applies, but it's there on their website.

## IDE (SEGGER Embedded Studio)

From the [SES downloads
 page](https://www.segger.com/downloads/embedded-studio/) you can
download SES for ARM -- it's available for Windows, MacOS and Linux.
I'm working on Linux, and the download is about 344 Mb.

The install bundle for Linux comes as a single tarball containing an
installation program and instructions. The installation progra has to
be run as root, which is not so good. (I think the root requirement is
just because the installer sets up some `udev` rules for talking to
J-Link debuggers. You can see them in
`/etc/udev/rules.d/99-jlink.rules` after installation.)

The installation takes up 1.2 Gb of disk space once it's there, and
includes builds of GCC and LLVM for ARM. The first time you start the
Embedded Studio application, you're offered the option of activating a
free license to use the software for development with Nordic devices.

You also need to download the nRF5 SDK, which comes in a confusing
array of versions. For Bluetooth-only development, you can use the
vanilla nRF5 SDK (current version is 16.0.0). You also need to get a
couple of "SoftDevices", Nordic's name for binary-only drivers they
provide for some networking functionality. You can get all this from
https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download

For developing applications that use Thread or Zigbee, you need to get
the nRF5 SDK for Thread and Zigbee instead. We'll deal with that
later...

The SoftDevice + nRF5 SDK download unpacks to about 750 Mb.


# Documentation

All of Nordic's developer documentation is on a slightly old-fashioned
looking website they call their Infocenter. It's not terrible, but
it's not that great either. For example, the board identifiers used
for Nordic development kits and dongles are things like PCA10056 (the
nRF52840 development kit), PCA10059 (the nRF52840 dongle) and so on.
There are some mysterious looking directories in the examples called
`pca10056e`. Searching in the documentation doesn't reveal anything
useful at all. Searching on the Nordic discussion forums finds one
note that it's something to do with an emulator running on some other
platform, but the link to the explanation from that note is broken.
There's lots of that sort of stuff, and it generally takes a long time
to find anything.


# Example 1: Blinky (`example-1`)

.../examples/peripheral/blinky/pca10056/blank/ses/blinky_pca10056.emProject

# Example 2: PWM blinky

## Pre-defined example (`example-2a`)

## Add PWM to basic blinky (`example-2b`)



# Example 3: BLE-controlled PWM blinky

## Pre-defined BLE example (`example-3a`)

## Add BLE to PWM blinky (`example-3b`)

## Add PWM blinky to BLE example (`example-3c`)



# Example 4: Thread + MQTT equivalent of BLE PWM blinky (`example-4`)



# Example 5: something "Almost Realistic" (`example-5`)



# The judging criteria

## Installation

**How easy is it to install the platform?**

**Is the download ridiculously large? How much disk space do you
need?**

**Does it work on Linux? Windows? MacOS? Any weird restrictions?**

**Is it free?**

## Quick start

**How long is "Zero To Blinky"?**

**Are there enough examples?**

**Does stuff just work?**

## Documentation

**Is there any?**

**Is there enough?**

**Is it any good? (i.e. not just Doxygen...)**

**Are there tutorials? Examples? Pictures? Performance data?**

## Basic workflow

### Edit

**Is there editor syntax support?**

**Do you have to use a specific IDE or can you use tools you're
 already familiar with?**

**If you have to use a specific IDE, is it any good?**

### Compile

**How easy is setting up paths to headers and libraries?**

**What are compiler error messages like?**

**Any extras (like warnings of potential power problems or things like
that)?**

### Flash

**Basically, does it work?**

### Debug

**What's the source level debugging like?**

**What are register and peripheral data views like?**

**Do breakpoints, watchpoints, etc. work? (I don't use these things a
   lot, but when you need them, you need them, so it's nice to know
   that they're there.)**

## Fancy workflows

**Command line builds: how easy are they to set up? are they possible
at all?**

**Testing: any special support?**

**Continuous integration: any special support?**

## Functionality

### Coverage of device functionality

**What device peripherals have driver libraries?**

**Are those libraries easy to use?**

**Are there any options missing?**

**If so, how easy is it to work around?**

### Configuration

**How easy is it to use different libraries or drivers in your code?**

**Are there any configuration or code generation tools to help with
the setup?**

### Libraries

**Are there higher-level libraries available for common functionality
 (e.g. communications, crypto, etc.)?**

**How easy is it to incorporate third-party code into your projects?**

### Frustration

**Basically: Did implementing the test programs make Ian angry?**
