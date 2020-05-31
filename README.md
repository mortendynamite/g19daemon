# G19daemon
Linux daemon for Logitech G19 Keyboard

G19daemon provides multiple virtual screens for the LCD on the Logitech G19 keyboard. It also allows the use of all additional keys of G19 keyboards, even if the kernel does not support them.

# Features
- Map G-keys to run special commands
- Change color of the keyboard backlight
- Show different information on the LCD
- Customize settings


# FAQ

## How to run daemon without root
- Install the application
- open bash
- run 'sudo usermod -a -G input <username>'
- After logoff and on you can run g19daemon from the command line or auto start it with your operating system.

## Where is the configuration saved?
All G19daemon settings are placed in ~/.config/G19Daemon/G19Daemon.conf

## Manually build G19daemon
Before you can build G19daemon you need some packages installed:
- libusb-1.0-0-dev
- libpulse-dev
- build-essential
- Qt
- Cmake

After installing all the above packages you can run 'sh ./build.sh'. The script will build the code and install it on the system.

## How to create own plugins
Every plugin must implement the plugininterface.hpp file (can be find on the root of the project directory.
Also after building the .so file the plugin need to be located into the plugin directory of the G19daemon installation (/usr/lib/g19daemon/0.1/plugins/).

Best is to use the helloworld plugin to make you familiar with the way the plugin system works.
