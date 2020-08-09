# system76-kbd-led

A simple keyboard LED controller for System76 laptops.

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

