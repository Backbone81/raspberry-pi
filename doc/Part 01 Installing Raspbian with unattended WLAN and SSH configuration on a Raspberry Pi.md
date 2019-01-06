Part 01: Installing Raspbian with unattended WLAN and SSH configuration on a Raspberry Pi
=========================================================================================
When experimenting with a Raspberry Pi, you often want to reset the operating system to a known state when something went wrong.
Doing this with the [NOOBS or Raspbian installer](https://www.raspberrypi.org/downloads/) by hand requires an unnessesary high
amount of time with plugging the Raspberry Pi to a screen, a keyboard and a mouse and then configuring SSH and WLAN. I wanted
this to be automated as much as possible.

The linx shell script at `raspbian-setup/setup.sh` is meant to run under Ubuntu Desktop 18.04 LTS. It downloads the Raspbian
Stretch operating system image, disables automount to prevent the operating system from immediately mounting the SD card after
the operating system has been written to it and writes the image file to the sd card.

For autoconfiguring the WLAN, you provide a `wpa_supplicant.conf` file in the root of the boot partition of the SD card.

For enabling SSH, you provide an empty `ssh` file in the root of the boot partition of the SD card.

With this script I am able to have a Raspberry Pi with a freshly installed Raspbian operating system within 10 minutes and
without messing with screen, mouse or keyboard cable. I can directly connect to the Raspberry Pi through SSH and adjust
whatever I need.

**NOTE: This part was only about how to kickstart your Raspberry Pi as fast as possible. You still have to secure your Raspberry
Pi by changing passwords, using public/private ssh keys or any other means. Securing your Raspberry Pi is very dependent on how
you want to use your device and is therefore out of scope of this documentation.**
