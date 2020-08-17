# system76-kbd-led

A simple keyboard LED controller for System76 laptops.

<u>*Associated Links*</u><br />
[![Trello Development Board](./assets/trello.png)](https://trello.com/b/6R6GS9bF)

**DISCLAMIER**: This is not a System76 development; it is a personal project of my own and for no reason should you contact System76 about this program for support or anything like that. It was created because I could not find a software tool for it provided by Pop, though one most likely exists somewhere, and I wanted a simple tool to control all of it via software. I'd love to get this merged into the Pop OS repositories if it's at all useful to them!

# Usage

`system76-kbd-led` controls the following sysfs nodes under the base System76 Keyboard LED prefix (`/sys/class/leds/system76::kbd_backlight`): `brightness`, `color_left`, `color_center`, `color_right`, `color_extra`.

```
usage: system76-kbd-led [-hlcretb]

Program options:
 -h             | Print help.
 -l <arg>       | Left color (rgb).
 -c <arg>       | Center color (rgb).
 -r <arg>       | Right color (rgb).
 -e <arg>       | Extra color (rgb).
 -t             | Toggle keyboard light (on or off).
 -b <arg>       | A negative or positive inc/dec value.
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

	$ sudo dpkg -i system76-kbd-led-*-Linux.deb

# Authors

* Kevin Morris &lt;kevr.gtalk@gmail.com&gt;

# License

This application is developed under the MIT public license, verbosely
located at `./LICENSE`.

