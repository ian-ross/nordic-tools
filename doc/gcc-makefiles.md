+++
title = "sky blue trades | Nordic Tools Comparison: GCC + Makefiles + nRF5 SDK (and similar)"
template = "project_page.html"
+++

# GCC + Makefiles + nRF5 SDK (and similar)

For the "real" work I did using the nRF52840 earlier this year, I
started off using the SEGGER tools, but I quickly switched to using a
Makefile-based build, simply because I found I was mostly editing the
SEGGER Embedded Studio project files by hand (see the [long and ranty
page](nrf5-sdk-ses) about those tools to see why). Doing this
insulates you from the annoyances of using SES, but it doesn't help
with the problems with the nRF5 SDK.

I'm not going to go over the whole example programs thing here,
because most of my complaints about the nRF5 SDK hold true for this
setup as well. I'll just say a little bit about the tools I ended up
using and something about how setting this all up goes.

As usual with these things, you need the `arm-none-eabi` GCC
cross-compiler build. The exact version doesn't seem to be critical.

To get started with a Makefile, you can take one from one of the nRF5
SDK examples. In each of these examples, there are Makefiles and GCC
linker scripts suitable for building things without getting involved
with SES. For example, in
`.../examples/peripheral/blinky/pca10056/blank/armgcc` you'll find a
Makefile and a file called `blinky_gcc_nrf52.ld`, which is a GCC
linker script. Turning the Makefile provided by Nordic into something
usable takes a little bit of work. As usual, Make brings out the worst
in people, and the tendency to overcomplicate things has been given
free rein here. I basically just disentangled *all* of the "macros
defining macros defining goals" nonsense that was going on here. It's
not helpful. The Nordic makefiles *are* useful for figuring out
compiler options and suitable preprocessor definitions to pull in the
right board definitions. The linker scripts are also handy, especially
if you want to use Nordic's "softdevice" binary drivers for Bluetooth,
since they need a fairly careful linker setup.

Once you have a nice flat and simple makefile, building goes like it
does for more or less any other embedded platform.

For flashing code to the development kit or dongle, you can use
Nordic's nRF Connect for Desktop application, and for debugging you
can use GDB connected to the SEGGER J-Link GDB server (which is
included in the installation bundle for the J-Link software). That
works as you'd expect: plug your dev board into a USB port, start the
debug server (you need to set up some options to make it talk to the
nRF52840's SWD port properly), then say `target remote localhost:3333`
in GDB to connect to the server. Breakpoints, source-level debugging
and all that kind of thing work fine, including sticky things like
catching hardfaults. (The nRF5 SDK includes a little library to help
with that, which was useful.)

You can also view debug output via the J-Link's RTT viewer. That's
pretty handy.

All in all, if you definitely have to use the nRF5 SDK, this is the
way I would go with it. You could do the same sort of thing using VS
Code or some other flexible IDE, but I just used Emacs, GDB and the
SEGGER J-Link tools and was pretty happy with that. (Or as happy as it
was possible to be while using the nRF5 SDK, which wasn't very happy!)

One additional comment here though: I was using OpenThread, and
getting that set up properly was an enormous pain in the ass. I'll
probably write a separate blog article about that, because getting
everything just right took a lot of messing around. I count 13
libraries I needed to link with, plus an indeterminate number of nRF5
SDK source files that needed to be included, plus some compile flags
that need to be just right. I was eventually able to get an
application working that used OpenThread, FreeRTOS and some nRF52840
peripherals and it worked fine, but it was a battle.
