# system76-kbd-led

A simple keyboard LED controller for System76 laptops supported by systemd services for color and brightness restoration on boot.

<u>*Associated Links*</u><br />
[![Trello Development Board](./assets/trello.png)](https://trello.com/b/6R6GS9bF)

**DISCLAMIER**: This is not a System76 development; it is a personal project of my own and for no reason should you contact System76 about this program for support or anything like that. It was created because I could not find a software tool for it provided by Pop, though one most likely exists somewhere, and I wanted a simple tool to control all of it via software. I'd love to get this merged into the Pop OS repositories if it's at all useful to them!

# Usage

`system76-kbd-led` controls the following sysfs nodes under the base System76 Keyboard LED prefix (`/sys/class/leds/system76::kbd_backlight`): `brightness`, `color_left`, `color_center`, `color_right`, `color_extra`.

```
usage: system76-kbd-led [-htx] [-l <arg>] [-c <arg>] [-r <arg>] [-e <arg>] [-b <arg>]

Program options:
 -h             | Print help.
 -t             | Toggle keyboard light.
 -x             | Restore colors and brightness.
 -l <arg>       | Left color (rgb).
 -c <arg>       | Center color (rgb).
 -r <arg>       | Right color (rgb).
 -e <arg>       | Extra color (rgb).
 -b <arg>       | A brightness increment value (eg. -50, 100, +100).
```

**Note**: The `-t` option uses a software cache, located at `/var/cache/system76-kbd-led/brightness`, which is initially populated with `/sys/class/leds/system76::kbd_backlight/brightness_hw_changed`.

# Building

`system76-kbd-led` is straightforward Linux application that uses CMake
with `gcc` or `clang` for it's build process.

	# Dependencies
	cmake
	gcc or clang

Building is very simple:

	$ mkdir build && cd build
	$ cmake -DCMAKE_BUILD_TYPE=DEBUG ..
	$ make
	# Run the program.
	$ ./src/system76-kbd-led -h

# Installation

Installation is straight forward; we recommend using CPack to generate
a package for your distribution.

### Debian

Generate a `.deb` package after building the project.

	$ mkdir build
	$ cd build
	$ cmake -DCMAKE_BUILD_TYPE=RELEASE ..
	$ make
	$ cpack -G DEB
	# system76-kbd-led-<major>.<minor>.<patch>-Linux.deb is generated.

Install it on a Debian-based operating system.

	# Install the .deb package.
	$ sudo dpkg -i system76-kbd-led-*-Linux.deb

Enable systemd services for color and brightness management on boot
and shutdown.

	# For color and brightness restoration.
	$ sudo systemctl enable system76-kbd-led.service
	# For color and brightness persistance when changed by hardware.
	$ sudo systemctl enable system76-kbd-led-cache.service

# Authors

* Kevin Morris &lt;kevr.gtalk@gmail.com&gt;

# License

This application is developed under the MIT public license, verbosely
located at `./LICENSE`.

