#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

// Filesystem prefixes.
#define SYSFS_PREFIX "/sys/class/leds/system76::kbd_backlight/"
#define SYSFS_COLOR_PREFIX SYSFS_PREFIX "color_"

// Some useful sysfs paths.
#define SYSFS_BRIGHTNESS_PATH SYSFS_PREFIX "brightness"
#define SYSFS_HW_BRIGHTNESS_PATH SYSFS_PREFIX "brightness_hw_changed"
#define SYSFS_MAX_BRIGHTNESS_PATH SYSFS_PREFIX "max_brightness"

// Utility functions and macros.
#define JOIN(prefix, value) prefix value

char *join(const char *prefix, const char *suffix);

int fs_exists(const char *path);
FILE *fs_open(const char *path, const char *modes);
int fs_size(FILE *f);
void fs_close(FILE *fd);

char *dirname(const char *path);

#endif /* FS_H */
