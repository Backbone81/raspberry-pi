Example 02 Button
=================
This project provides a device driver for detecting a button press
and signaling this button press to an other GPIO pin with the help
of a hardware interrupt handler.

For the Makefile to build this project, you need the linux kernel header files
installed. On Raspbian this requires the apt package "raspberrypi-kernel-headers".

The shell script files "load.sh" and "unload.sh" can be used to install and
uninstall the driver temporarily.

The "driver.c" file is the main entry point for the linux device driver.

The Visual Studio project uses a Linux Makefile project for developing on Windows
but building on Linux.

The device driver provides a file at /dev/example02-button but it
has no functionality. It is just a stub for extending it later.
