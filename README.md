# system76-kbd-led

A simple keyboard LED controller for System76 laptops supported by systemd services for color and brightness restoration on boot.

<u>*Associated Links*</u><br />
[![Trello Development Board](./assets/trello.png)](https://trello.com/b/6R6GS9bF)

**DISCLAMIER**: This is not a System76 development; it is a personal project of my own and for no reason should you contact System76 about this program for support or anything like that. It was created because I could not find a software tool for it provided by Pop, though one most likely exists somewhere, and I wanted a simple tool to control all of it via software. I'd love to get this merged into the Pop OS repositories if it's at all useful to them!

# Usage

`system76-kbd-led` controls the following sysfs nodes under the base System76 Keyboard LED prefix (`/sys/class/leds/system76::kbd_backlight`): `brightness`, `color_left`, `color_center`, `color_right`, `color_extra`.

```
usage: system76-kbd-led [-h,--help] [-v,--verbose] [-t,--toggle] [-x,--restore] [-l,--left <arg>] [-c,--center <arg>] [-r,--right <arg>] [-e,--extra <arg>] [-b,--brightness <arg>] [-i,--increment <arg>]

Program options:
  -h [ --help ]           Display the help message.
  -v [ --verbose ]        Enable debug logging.
  -t [ --toggle ]         Toggle keyboard.
  -x [ --restore ]        Restore colors and brightness.
  -l [ --left ] arg       Left color (rgb).
  -c [ --center ] arg     Center color (rgb).
  -r [ --right ] arg      Right color (rgb).
  -e [ --extra ] arg      Extra color (rgb).
  -b [ --brightness ] arg Brightness overriding value.
  -i [ --increment ] arg  Brightness increment (-/+).
```

**Note**: The `-t` option uses a software cache, located at `/var/cache/system76-kbd-led/brightness`, which is initially populated with `/sys/class/leds/system76::kbd_backlight/brightness_hw_changed`.

# Building

## Build Dependencies

### Debian/Ubuntu
| Package                      | Version  |
|  :---:                       |  :---:   |
| g++-10                       | >= 10.1  |
| libstdc++-10-dev             | >= 10.1  |
| libboost-program-options-dev | >= 1.65  |
| cmake                        | >= 2.8.8 |
| git                          | any      |


### Arch Linux
| Package    | Version  |
|  :---:     |  :---:   |
| g++        | >= 10.1  |
| libstdc++  | >= 10.1  |
| boost      | >= 1.65  |
| boost-libs | >= 1.65  |
| cmake      | >= 2.8.8 |
| git        | any      |

After the deps for your system are installed, you can proceed to the compilation step below. 

## Compilation

To build `system76-kbd-led`, proceed with the following process:

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

	$ git pull origin --tags # Fetch all tags from the repository.
	# (Required to produce the correct version for CPack's .deb)
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

