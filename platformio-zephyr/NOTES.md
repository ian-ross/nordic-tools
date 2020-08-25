Here, I'm going to try all this stuff using PlatformIO + Zephyr.
Fortunately, [Valerii Koval](https://github.com/valeros) already wrote
a tutorial about using Bluetooth on the nRF52840 in this setup, which
you can read
[here](https://docs.platformio.org/en/latest/tutorials/nordicnrf52/zephyr_debugging_unit_testing_inspect.html)!
I'll be looking in there for help with Bluetooth when we get to it.

What are PlatformIO and Zephyr? From the PlatformIO website:
"PlatformIO is a cross-platform, cross-architecture, multiple
framework, professional tool for embedded systems engineers and for
software developers who write applications for embedded products." And
from the Zephyr website: "The Zephyr Project strives to deliver the
best-in-class RTOS for connected resource-constrained devices, built
be secure and safe."

They are both what I think of as "maximalist" projects, aiming to
support *everything* *everywhere* (or more or less). When I first saw
them, I wasn't so keen on this idea, but after thinking about it a
bit, it's grown on me more. For comparison with Zephyr, think of
Linux. If you want an OS to run on any architecture with a paging MMU,
you use Linux. It supports more or less all hardware you can think of
(plus a lot you've never heard of), and it's a sort of universal
compilation target for that scale of system. Why not have something
similar for smaller systems?

My initial objection was that the architectures at the smaller end are
too variable, the peripherals are too variable, and the peripherals
generally aren't auto-discoverable like they are on larger
architectures. But the variability argument doesn't wash at all. Linux
runs on *everything* from little ARM processors (and some people even
squash it onto STM32s), up to IBM z/Architecture monsters and
supercomputers. The variability in peripherals is just as huge. The
non-discoverability argument is a little tougher, but adaptation of
things like Linux's devicetree system can work really well for smaller
architectures (and that's what Zephyr does).

PlatformIO takes all this one step further, integrating multiple
toolchains across different platforms, manufactures, operating
systems, APIs, etc. into a single relatively simple IDE front-end that
helps with all the installation and configuration of SDKs. One of the
platforms that it supports is Zephyr.

Sounds good. Let's give it a whirl!


# Installation

PlatformIO comes in two parts, a command line interface and an IDE.
For most applications, you only need to install the IDE part, because
you can get that to install the command line stuff for you later (you
might need to install the command line tools separately for CI
systems, for example). The PlatformIO IDE is based on Visual Studio
Code instead of being a hand-rolled "new IDE". Which I think is an
excellent choice. Even if I'm not normally an IDE user, if there's
going to be an IDE, I would *much* rather it be an existing
well-tested and well-liked one than something that some new untested,
probably poorly designed IDE. There are plugins for other IDEs and
editors, but let's go with VS Code here.

Installation is simple:

1. Install VS Code (from your OS package manager or by downloading
   from the [VS Code site](https://code.visualstudio.com/)).

2. In the VS Code Extension Manager, search for "PlatformIO IDE".

3. Press the green button! You need to wait a while as VS Code
   installs some dependencies it needs before downloading the main
   PlatformIO install.

4. Restart VS Code and the PlatformIO bug logo will appear in the
   left-hand icon bar. You're all set!

That's remarkably easy. You end up with a 60 Mb `~/.platformio`
directory in your home directory, but that seems to be about all.
PlatformIO downloads the other platform-dependent tools it needs as
you request them, which means you don't have to install *everything*
right away. The first time you create a project for a new board,
PlatformIO will download the board support package for the board. The
first time you use a particular framework for a project (e.g. Zephyr),
PlatformIO will download what's needed.

After setting up a project using Zephyr for the nRF52840 development
kit, the contents of the `~/.platformio` directory have grown to
1.4Gb, but that includes a full GCC toolchain, other build tools, a
full Zephyr distribution, plus a pile of other Zephyr-friendly
libraries (including OpenThread). For what's there, it doesn't seem
like too much.


# Documentation

There's a lot of documentation, though it's quite hard to assess how
much there is and how useful it is because it's quite naturally split
between documentation for the PlatformIO system and documentation for
Zephyr.

After a bit of doing things with all this, the Zephyr documentation
really doesn't look too bad. Also, it's relatively easy to spelunk in
the `Kconfig` and devicetree files for the different boards that are
supported to see how things are set up. All of these things end up
under `~/.platformio/packages/framework-zephyr` when you first set up
a PlatformIO project using Zephyr.

One problem with Zephyr is just its size. Think of it as being
comparable to Linux and you won't go far wrong. It has APIs for
*everything*, supports hundreds of boards and many MCU architectures.
I can't do it anything like justice in this sort of fly-by test. All I
can say is that it looks like there's enough documentation to be going
on with, and there's a *lot* of stuff in the Zephyr project. The core
framework as downloaded by PlatformIO is about 1.5M lines of code
(very roughly), then there are versions of other things packaged for
use with Zephyr, like OpenThread, LVGL, mbedtls, and so on.


# Example 1: Blinky (`example-1`)

One of the things that ends up being downloaded with the nRF52840
development kit BSP is some examples. You can find a Zephyr-based
blinky example in
`~/.platformio/platforms/nordicnrf52/examples/zephyr-blink`. (There
are also examples within the Zephyr framework directories.)

To get going, I created a new Zephyr project for the nRF52840
development kit in PlatformIO, then copied the contents of the
`src/main.c` from that example into my project.

And then, you plug your nRF52840 development kit into a USB port and
you choose one of the "Run" options from the "Debug" menu and
PlatformIO:

 * downloads the SEGGER J-Link tools,
 * compiles your code and the Zephyr library,
 * links everything for the nRF52840 development kit,
 * flashes the result to the board and
 * drops into the debugger at the top of your `main` function.

It just works, with no messing around required at all. Fantastic
stuff!


# Example 2: PWM blinky

## Pre-defined example (`example-2a`)

There is a pre-defined PWM blinky example in the Zephyr distribution,
which can be found in
`~/.platformio/packages/framework-zephyr/samples/basic/blinky_pwm`.
Getting this to work with PlatformIO looks like a useful thing to do,
since it uses the Zephyr `Kconfig`-based configuration system to
switch on PWM support.

This ended up being a little bit confusing. Initially, I just copied
the source code from the Zephyr example into a new PlatformIO project
set up to use Zephyr on the nRF52840 development kit, and the code
appeared to build without any problems at all, even though there
wasn't any additional Zephyr configuration set up (which usually goes
in a `prj.conf` file). But when I tried uploading the code to the
nRF52840 board with VS Code's built-in debugger, I got a bunch of
weird compilation errors. After some messing around (manually deleting
`.pio` directories, setting up the necessary `zephyr/prj.conf` file
which really *was* needed, rebuilding from scratch), I got something
working.

I'm sure this would all be easier if I wasn't also a complete beginner
at using VS Code. I really do want to like it, but I'm not getting
there yet. The problems with it are basically the same as you get in
any of these "integrated all the things" environments, where details
are hidden and hard to discover. PlatformIO has a build tool called
`pio`, Zephyr has a build tool called `west`, and they all run inside
of VS Code when you press some buttons. It's all a bit undiscoverable.
At least if you have makefiles, even if the person who wrote the
makefiles is a sadistic idiot, you can eventually track down what's
doing what. I find that much harder to do in these integrated
environments.

Also, two extra niggles. First, the serial monitor in VS Code doesn't
seem to understand ANSI terminal control sequences. Some of the Nordic
libraries use these in their debug output, and they get a bit mangled
in the VS Code terminal. I think I'll stick to Minicom there. Second,
the serial debugging doesn't work by default: you need to create a
`prj.conf` file with it set up, and you need to configure the serial
monitor speed in the `platformio.ini` file. These seem like things
that should be set up by default.

Anyway, after a bit of hassle there was some PWM action going on.

## Add PWM to basic blinky (`example-2b`)

Next thing is to make a copy of the basic blinky and try to make it
into a PWM blinky.

First step: add some debug output to the blinky code using Zephyr's
`printk` function. That means setting up the necessary `prj.conf`
things to make that work. In a standalone Zephyr installation, you can
use the usual `Kconfig` user interfaces to do this (you do something
like `west build -t guiconfig`, for example). Unfortunately,
PlatformIO doesn't allow you to do this. There's no support for the
`Kconfig` tools, and it appears that the way that the PlatformIO build
directories are arranged means that you can't even get around it by
using a lower-level approach (which is running the Ninja build tool
after the main Zephyr build process has built some stuff for you
first).

That is, to be honest, a very serious disappointment. The
`Kconfig`-based approach to system configuration in Zephyr is one of
its biggest selling points for me, and the easy discoverability of
configuration settings that you get from the `menuconfig` and
`guiconfig` tools is a huge win. Losing that is a bit of a
blow, since it means you have to go fossicking through hundreds of
`Kconfig` files to work out what options are available and how to
switch them on. (Not kidding about the "hundreds": there are actually
1540 `Kconfig` files under the `~/.platformio` directory in my
installation right now!)

Still, let's press on. You need to add something like the following to
`prj.conf` to make `printk` work within VS Code:

```
CONFIG_PRINTK=y
CONFIG_STDOUT_CONSOLE=y
CONFIG_LOG=y
```

Incidentally, this is another thing I'm not completely convinced by
with this kind of integrated environment: where exactly is that
`printk` output going? Is it coming over the SEGGER RTT link? Or is it
going somewhere else? You can find out by some exploration in the
Zephyr documentation to identify the relevant configuration options,
and then by looking in the board configurations in the
`~/.platformio/framework-zephyr` tree. The file
`~/.platformio/packages/framework-zephyr/boards/arm/nrf52840dk_nrf52840/nrf52840dk_nrf52840_defconfig`
defines `USE_SEGGER_RTT=y`, which causes log output to go the SEGGER
RTT console. Not a completely obvious thing to have to do though.

That file is handy to look into anyway, since it tells you what
`Kconfig` things you don't need to set up because they're configured
by default if you're using the nRF52840 development kit. In the same
directory is a devicetree definition for the board, which is also
useful to see. It includes definitions that set things up so that only
one (of four) PWM channels is connected to an LED output. I'm not sure
what the other PWM channels are used for, or if there's any way to
access them from within Zephyr.

In any case, the code changes to make the blinky into a "soft fade"
PWM blinky are pretty easy. The PWM blinky example from the Zephyr
distribution uses a function called `pwm_pin_set_usec` to set the PWM
duty cycle, but there's also `pwm_pin_set_nsec` that takes the PWM
period and on-time in nanoseconds, so it was possible to set up a 40
kHz PWM frequency and to get super-smooth fading.

The only configuration change needed to make it work is to add
`CONFIG_PWM=y` to the `prj.conf` file.

That's actually pretty good. And a lot of the pain with it can be put
down to unfamiliarity with the platform, the tools, the IDE, the
everything. Score one to Zephyr!


# Example 3: BLE-controlled PWM blinky

So, PWM is pretty basic. Let's see if we can do some Bluetooth.

## Pre-defined BLE example (`example-3a`)

Let's give an environmental sensing example from the Zephyr
distribution a try. It's in `samples/bluetooth/peripheral_esp`.

So, we make a new PlatformIO Zephyr project, copy in the `main.c` and
`prj.conf` file from the example, hit "Build", the hit "Upload" to
deploy onto the nRF52840 development kit.

And it works! I can connect to the device using the Android nRF
Connect Bluetooth debugging application, can see the attributes
supported by the example, pull values, and so on.

That was almost disappointingly undramatic. The `prj.conf` file has 9
lines of Bluetooth-specific configuration information. The `main.c`
application code is about 450 lines, and there's nothing particularly
surprising about it. There are some macros to define the attributes
supported, then everything runs off of a few callbacks.

I think that this is now starting to get at the real benefit of
Zephyr. All of the complexity of setting up Bluetooth on the nRF52840
is more or less gone. You only have to write code that corresponds to
Bluetooth-level entities, and you don't need to worry about dealing
with the Nordic "SoftDevice" Bluetooth drivers at all. It's nice.


## BLE PWM blinky (`example-3b`)

One problem here was that the Bluetooth example I used as `example-3a`
is for an environmental sensing service, not a UART service. The
Zephyr Bluetooth library doesn't seem to include a UART service at
all, which is a little surprising. Even though the UART service is a
Nordic custom service, it's quite widely supported by other platforms
(e.g. CircuitPython, and I think ARM mbed too).

What this means is that I had to take a different approach here to the
other platforms I've been testing, and write a custom Bluetooth
service with a single GATT characteristic for the PWM blinky duty
cycle. This means that the two steps "Add BLE to PWM blinky
(`example-3b`)" and "Add PWM blinky to BLE example (`example-3c`)" are
folded into one here, which I've labelled as `example-3b`.

This effort was made a little more difficult by two factors:

1. I don't know a whole lot about Bluetooth. Basically just stuff I've
   picked up by reading example code during this project... I'd not
   done any Bluetooth work at all before starting this, so I don't
   really know what I'm doing.

2. The Zephyr documentation for their Bluetooth library isn't all that
   good. This is kind of the flip side of the wide support for a lot
   of complex subsystems within Zephyr. It's already a pretty huge
   effort to get something like Zephyr going. To document everything
   in detail multiplies that effort by a non-trivial factor. That
   means that some areas just have sparse Doxygen-like documentation.
   The Bluetooth library has a set of macros for setting up GATT
   profiles, and it would have been useful to have some sort of
   tutorial documentation for how to use them. Instead I "guessed and
   messed", based on the example code I found in the Zephyr install.
   (At least there's a fair amount of example code!)

Point #1 here obviously neutralises any serious complaining I could do
about point #2! I've not reviewed all of the Zephyr documentation, but
it does seem to be a little uneven, which is not so surprising.
Although Zephyr is aiming to be the "Linux of the embedded world",
it's going to take some time for the standard of documentation to get
up to the standard of the Linux kernel documentation (which is
outstanding).

All that said, it wasn't that difficult to get a basic Bluetooth
custom service going. I had one minor wobble with getting the service
advertising data right, but I think that was mostly down to
unfamiliarity with Bluetooth, not any deficiency in Zephyr.

This was a good test of the ease of debugging in PlatformIO too. I
wrote a very basic custom Bluetooth service with a single integer
characteristic and read and write callbacks for that characteristic
that just logged messages. Those messages are printed with Zephyr's
`printk` function, which goes to whatever debug console the system
under test supports, and the messages appear in the PlatformIO's
serial monitor panel in VS Code. While that's going on it's also easy
to single-step through things in the debugger, and because of the way
that Zephyr builds work, all the Zephyr source that your project uses
is directly available, debuggable and searchable. That's very nice,
and makes it easy to track problems down.

Once I got the advertising working, I could talk to the Bluetooth
service using the Android nRF Connect application: I could read my
custom characteristic and see a message appear in the PlatformIO
serial monitor from the read callback, and so on.

Once things were at that point, it only took five minutes to
incorporate the PWM blinky side of things to get a working
application. That was really smooth.


# Example 4: something "Almost Realistic" (`example-4`)

Here, we're going to start from the Bluetooth PWM blinky example from
above and add some extra functionality. We want this to do the
following:

1. Replicate the PWM blinky behaviour from `example-3b`.
2. Calculate and publish digits of &pi; via another Bluetooth
   characteristic.
3. When a button is pushed on the nRF52840 dev kit, toggle the digit
   generation and publication, and flash the PWM LED a few times for
   feedback.





# The judging criteria

## Installation

**How easy is it to install the platform?**

Installation is easy if you use VS Code, because the whole of
PlatformIO can be installed as a VS Code extension. Once that's done,
PlatformIO downloads and installs packages and libraries as needed
when you use them in a build. It's very convenient.

**Is the download ridiculously large? How much disk space do you
need?**

After getting to the end of Example 3, i.e. having everything in place
to be able to do Bluetooth things, there was a little less than 2 Gb
in my `~/.platformio` directory. That doesn't seem too bad. Like all
these things, it includes a full GCC toolchain as well as all the
source code for Zephyr, tools like the SEGGER J-Link utilities, and so
on. These self-contained installs might not be the most efficient
thing for disk space, but they make life a lot simpler than trying to
do everything with operating system packages.

**Does it work on Linux? Windows? MacOS? Any weird restrictions?**

Yes, yes and yes, I think. PlatformIO claims to be cross-platform, and
I have no reason to disbelieve that, since it's based on VS Code.
Zephyr is just a set of C libraries that you cross-compile, so if your
cross-compiler toolchain works, you should be golden.

**Is it free?**

Yes. PlatformIO is free and open-source, as is Zephyr.

## Quick start

**How long is "Zero To Blinky"?**

Not very long at all.

**Are there enough examples?**

There are lots of examples.

**Does stuff just work?**

Mostly, as far as I could tell from my testing. The only places where
you have to make special effort are the places where Zephyr support is
still thin, or where the documentation is lacking. I'm sure those will
get better with time.

## Documentation

**Is there any?**

There is.

**Is there enough?**

Nope. If Zephyr really is going to be "the Linux of the embedded
world", it needs the same standards of documentation as Linux has. And
those standards are very high.

**Is it any good? (i.e. not just Doxygen...)**

A lot of it is unfortunately just Doxygen documentation, often with
nothing to connect the Doxygen pages at all. Fortunately there are
lots of examples, so you do have somewhere to get started.

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
