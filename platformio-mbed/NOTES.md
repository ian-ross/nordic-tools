# Installation

This was a total failure.

There are two routes to installing ARM mbed. One is to install the
mbed Studio software. Unfortunately, for Linux, this supports only a
couple of older Ubuntu versions (16.04 or 18.04), so it's not an
option (I use Arch Linux). Why an IDE is so specific about OS versions
I couldn't say, but it's not a good sign.

The second option is to use PlatformIO, and select mbed as the platform.

This superficially worked, and downloaded about 2 Gb of stuff, and a
blinky program (taken from the mbed website) compiled (after a very
long process of compiling every *single* source file in the mbed
distribution) and claimed to download to the nRF52840 development
board. But it didn't run. And debugging didn't reveal anything
obvious, apart from the fact that the code seemed to hang in a line
that said "`led1 = !led1;`", with `led1` being a C++ objects referring
abstracting one of the LEDs on the nRF52840 development board.

And there we come to one of the problems of these highly integrated
systems. If I'd been using GCC + OpenOCD + GDB and something didn't
work, I'd quickly be able to figure out what the problem was. Here,
that's not so easy.

(Also, there's something up with the build dependencies for mbed under
PlatformIO. Every single mbed source file gets rebuilt whenever you do
anything. It's not usable. A 10-line blinky program should compile in
a few seconds, not take multiple minutes to build.)

So, blinky don't blink. That's pretty fatal.

I could try to work out what's going on, but life is too short. Nordic
aren't involved in mbed development any more as far as I can tell, so
I don't think it's really a viable option for development on their
devices. (I could be wrong here, since I've not spent time to
investigate in detail.)

I'm not going to go any further with mbed.
