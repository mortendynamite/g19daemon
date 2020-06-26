# G19daemon
Linux daemon for Logitech G19 Keyboard

G19daemon provides multiple virtual screens for the LCD on the Logitech G19 keyboard. It also allows the use of all additional keys of G19 keyboards, even if the kernel does not support them.

# Features
- Map G-keys to run special commands
- Change color of the keyboard backlight
- Show different information on the LCD
- Customize settings
- Plugin support  
    - Clock
    - Media Player (mris)
    - [Hardware monitor](https://github.com/lonelobo0070/Hardware-Monitor-Applet)
    - Blacklight color changer
    - LCD brightness changer
    - Hello world
    - Pulseaudio volume

# Changelogs

For the full detailed changelog you can look in [changelog section](Changelog.md).


# FAQ

## How to install G19daemon

Unsigned deb and rpm files are generated on every release. Those can be used to install the application by using the package manager of your operating system. The files can be found on the releases package of the repository.

You can manually build and install from source. More info can be found in the [Manually build G19daemon](#Manually-build-G19daemon') section in the FAQ.

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

After installing all the above packages you can run 'sh ./mb.sh && sudo make install'. The script will build the code and install it on the system.

## How to configure G-keys to open applications?
When starting the g19daemon a system tray icon is show. When you right click on it you can click 'show'. This will open the G19daemon configurator.
In the configurator all the G-keys and M-keys are shown. Here you can place all the commands of the application/scripts that you want to open with the G-keys.

For example I want the M1-G1 key to open Chrome. I place '/usr/bin/google-chrome-stable %U' in the G1 textfield in the M1 tab. After saving this settings you can press the G1 key and Chrome will be opened.

To easily find the commands of the installed application you can use for example 'KDE Menu editor', ...

## How to create own plugins
Every plugin must implement the plugininterface.hpp file (can be find on the root of the project directory.
Also after building the .so file the plugin need to be located into the plugin directory of the G19daemon installation (/usr/lib/g19daemon/0.1/plugins/).

Best is to use the helloworld plugin to make you familiar with the way the plugin system works.
