+++
title = "The Judging Criteria"
[taxonomies]
categories = ["embedded"]
+++

# The judging criteria

### Installation

 - How easy is it to install the platform?
 - Is the download ridiculously large? How much disk space do you
   need?
 - Does it work on Linux? Windows? MacOS? Any weird restrictions?
 - Is it free?

### Quick start

 - How long is "Zero To Blinky"?
 - Are there enough examples?
 - Does stuff just work?

### Documentation

 - Is there any?
 - Is there enough?
 - Is it any good? (i.e. not just Doxygen...)
 - Are there tutorials? Examples? Pictures? Performance data?

### Basic workflow

The stuff you do every day:

 - Edit: is there editor syntax support? do you have to use a specific
   IDE or can you use tools you're already familiar with? if you have
   to use a specific IDE, is it any good?
 - Compile: how easy is setting up paths to headers and libraries?
   what are compiler error messages like? any extras (like warnings of
   potential power problems or things like that)?
 - Flash: basically, does it work?
 - Debug: what's the source level debugging like? what are register
   and peripheral data views like? do breakpoints, watchpoints, etc.
   work? (I don't use these things a lot, but when you need them, you
   need them, so it's nice to know that they're there.)

### Fancy workflows

Stuff you might not do every day, but that you might want to (testing
and CI you can set up yourself as long as you can do command line
builds without too much trouble, but extra support for them is sure
nice to have):

 - Command line builds: how easy are they to set up? are they possible
   at all?
 - Testing: any special support?
 - Continuous integration: any special support?

### Functionality

 - Coverage of device functionality: what device peripherals have
   driver libraries? are those libraries easy to use? are there any
   options missing? if so, how easy is it to work around?
 - Configuration: how easy is it to use different libraries or drivers
   in your code? are there any configuration or code generation tools
   to help with the setup?
 - Libraries: are there higher-level libraries available for common
   functionality (e.g. communications, crypto, etc.)? how easy is it
   to incorporate third-party code into your projects?

### Frustration

Bascially: Did implementing the test programs make Ian angry?
