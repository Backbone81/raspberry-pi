Part 02: Using Visual Studio 2017 for developing C/C++ applications on a Raspberry Pi
=====================================================================================
Because I have more experience with developing C/C++ applications on Microsoft Windows with Visual Studio that on Linux,
I wanted to use Visual Studio 2017 on my Windows 10 developer machine for developing C/C++ applications on the Raspberry Pi.
For this to work, you have to add the workload for Linux development with C++ on the Visual Studio 2017 installer.

With that workload installed, you can create new projects with the type Visual C++/Cross Platform/Linux/Console Application (Linux).
To make this project work with a Raspberry Pi, you have to change the target from x64 to ARM. Note that although the Raspberry Pi 3
Model B+ has a 64 bit processor, Raspbian is still only 32 bit. This means you cannot use the ARM64 project target for a Raspberry Pi.

When you first run your C/C++ application from within Visual Studio, you will get prompted for SSH connection information. Provide
the required data for your Raspberry Pi and Visual Studio will copy your source code over to your Raspberry Pi and compile it with
the GCC available there. The Visual Studio debugger wil even connect to the debugger on the Raspberry Pi and allows you to set
breakepoints and step through your application, while it is running on the Raspberry Pi. Make sure you show the Linux Console Window
from the Debug menu. Otherwise you cannot see what your application did when your debugging session ends.

The SSH connection to your Raspberry Pi is managed under Tools-Options-Cross Platform-Connection Manager. You can add or remove
additional connections as you need them.

Another nice thing is that Visual Studio will download all C/C++ header files from your Raspberry Pi and make them available for
IntelliSense. This means code completion will work as if you were developing a native Windows application. But this feature has a
default setting which does not work well with the Raspberry Pi and will fail to retrieve those header files. Open the cache directory
for those header files by going to the Options-Cross Platform-Connection Manager-Remote Headers IntelliSense Manager and choose the
button "Explore". This will open up the Windows Explorer at the right directory. There you will find a file named `settings.xml.unused`.
Copy this file to `settings.xml` and change its content in a text editor. Change the entry for `syncMethod` from `rsync_ssh` to `sftp_ssh`.
Make sure that you have the `zip` package installed on your Raspberry Pi. Afterwards update the header files through the Remote Headers
IntelliSense Manager and you should be fine.

On Linux systems you usually do not want to log into your system as root. Instead you use a custom user with sudo privileges. But for some
development scenarios we will see later (accessing physical memory), you have to run your application as root and Visual Studio does not
support using `sudo`. For those situations you may want to enable the root user to connect to your Raspberry Pi with SSH. But make sure
your Raspberry Pi is on a private WLAN and is not accessible from the internet. Otherwise this can be a serious security problem.
