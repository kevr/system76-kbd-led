#ifndef FS_HPP
#define FS_HPP

#include <fstream>
#include <string>

#ifndef SYSFS_PREFIX
#define SYSFS_PREFIX "/sys/class/leds/system76::kbd_backlight/"
#endif

#ifndef JOIN
#define JOIN(a, b) a b
#endif

namespace fs
{

/**
 * @brief Check to see if a file exists or not.
 *
 * This function is implemented via POSIX access.
 *
 * @param path Path to file to check.
 * @returns Boolean indicating whether we found a file at path or not.
 **/
bool exists(const std::string &path);

/**
 * @brief Open a file at path for modes.
 *
 * A wrapper for std::fstream::open.
 *
 * @param path Path to file to open.
 * @param modes Modes to open the file in.
 * @returns A new std::fstream object constructed with {path, modes}.
 **/
std::fstream open(const std::string &path, std::ios::openmode modes);

}; // namespace fs

#endif /* FS_HPP */
