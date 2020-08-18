/**
 * File: main.c
 *
 * The main entry point for system76-kbd-led. This program serves as
 * a software controller for the System76 keyboard LED lights under
 * the Linux kernel. It takes hardware brightness levels into account
 * when needed and saves a software cache for brightness and colors
 * to restore on boot or when toggling on/off.
 *
 * Author: Kevin Morris <kevr.gtalk@gmail.com>
 * License: MIT (see ./LICENSE)
 *
 * Trello: https://trello.com/b/6R6GS9bF/system76-kbd-led
 **/
#include "buffer.h"
#include "cache.h"
#include "colors.h"
#include "fs.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Return codes.
#define SYSFS_OPEN_FAILED 2
#define INVALID_ARGUMENT 1

// Cache files.
#define CACHE_PATH "/var/cache/system76-kbd-led/brightness"
#define HW_CACHE_PATH "/var/cache/system76-kbd-led/hw_brightness"

// Shell functions.
int print_usage(int argc, char *argv[]);
int print_help(int argc, char *argv[]);
int error(int return_code, const char *message);

// Branch functions.
int do_brightness(int increment);
int do_toggle(void);
int do_color(const char *names[], char *colors[]);

int main(int argc, char *argv[])
{
    int opt;
    char *left = NULL, *center = NULL, *right = NULL, *extra = NULL;
    char toggle = 0;
    int increment = 0;
    char restore_colors = 0;

    while ((opt = getopt(argc, argv, "htxl:c:r:e:b:")) != -1) {
        switch (opt) {
        case 'h':
            return print_help(argc, argv);
        case 't':
            toggle = 1;
            break;
        case 'x':
            restore_colors = 1;
            break;
        case 'l':
            left = optarg;
            break;
        case 'c':
            center = optarg;
            break;
        case 'r':
            right = optarg;
            break;
        case 'e':
            extra = optarg;
            break;
        case 'b':
            increment = strtol(optarg, NULL, 10);
            if (errno)
                return error(errno, strerror(errno));
            break;
        case '?':
            return print_usage(argc, argv);
        }
    }

    setup_cache();

    const char *names[] = {"left", "center", "right", "extra"};
    char *colors[CNT_COLORS] = {left, center, right, extra};

    // If -x was given, restore colors by reading a color array
    // from cache and passing it to do_color.
    if (restore_colors) {
        // Flag for read state.
        char success = 0;

        printf("Restoring brightness...\n");
        int hw_brightness = -1;
        read_int_bytes_unsafe(HW_BRIGHTNESS_CACHE, &hw_brightness, &success);
        if (success)
            write_int_bytes(SYSFS_BRIGHTNESS_PATH, hw_brightness);

        printf("Restoring colors...\n");
        char *restoration_colors[CNT_COLORS];
        read_color_cache_unsafe(restoration_colors, &success);
        if (success) {
            printf("Found color cache!\n");
            printf("Restoring colors...\n");
            do_color(names, restoration_colors);
            free_color_data(restoration_colors);
        } else {
            printf("No color cache found, skipping restoration.\n");
        }
    }

    // If -b (brightness) was given.
    if (increment != 0)
        do_brightness(increment);

    // Otherwise, restore brightness.
    int brightness = -1, sys_hw_brightness = -1, hw_brightness = -1;

    // Read /sys/class/leds/system76::kbd_backlight/brightness.
    read_int_bytes(SYSFS_BRIGHTNESS_PATH, &brightness);
    read_int_bytes(HW_BRIGHTNESS_CACHE, &hw_brightness);

    // If the current sys brightness is mismatched from our cache,
    // it must have been changed via hardware, so update it.
    if (brightness != 0 && brightness != hw_brightness)
        write_int_bytes(HW_BRIGHTNESS_CACHE, brightness);

    // If -t (toggle) was given.
    if (toggle)
        do_toggle();

    if (left || center || right || extra) {
        // Run through this every time: each NULL element is ignored by
        // do_color.
        do_color(names, colors);
    }

    // Populate colors with current left, center, right, and extra.
    int i;
    for (i = 0; i < CNT_COLORS; ++i) {
        char *path = join(SYSFS_COLOR_PREFIX, names[i]);
        char *color_value = get_color(path);
        free(path);
        colors[i] = color_value;
        printf("color(%s): %s\n", names[i], color_value);
    }

    // Write colors out to cache.
    write_color_cache(colors);
    free_color_data(colors);

    return 0;
}

int print_usage(int argc, char *argv[])
{
    printf("usage: %s [-htx] [-l <arg>] [-c <arg>] [-r <arg>] [-e <arg>] [-b "
           "<arg>]\n",
           argv[0]);
    return -1;
}

int print_help(int argc, char *argv[])
{
    print_usage(argc, argv);
    printf("\nProgram options:\n -h\t\t| Print help.\n -t\t\t| Toggle "
           "keyboard "
           "light.\n -x\t\t| Restore colors and brightness.\n -l <arg>\t| "
           "Left color "
           "(rgb).\n "
           "-c <arg>\t| Center color (rgb).\n -r <arg>\t| Right color "
           "(rgb).\n "
           "-e <arg>\t| Extra color (rgb).\n -b <arg>\t| A brightness "
           "increment "
           "value (eg. -50, 100, +100).\n\n");
    return 0;
}

int error(int return_code, const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    return return_code;
}

int do_brightness(int increment)
{
    const char *brightness_path = JOIN(SYSFS_PREFIX, "brightness");
    const char *max_brightness_path = JOIN(SYSFS_PREFIX, "max_brightness");

    int brightness = -1;
    read_int_bytes(SYSFS_BRIGHTNESS_PATH, &brightness);

    int max_brightness = -1;
    read_int_bytes(SYSFS_MAX_BRIGHTNESS_PATH, &max_brightness);

    const int LOWER_BOUND = 45;
    const int UPPER_BOUND = max_brightness;

    int new_value = brightness + increment;
    if (new_value < LOWER_BOUND)
        new_value = LOWER_BOUND;
    else if (new_value > UPPER_BOUND)
        new_value = UPPER_BOUND;

    // Write out the new brightness value to everything:
    // sysfs brightness, brightness cache and hw_brightness cache.
    write_int_bytes(SYSFS_BRIGHTNESS_PATH, new_value);

    return 0;
}

int do_toggle(void)
{
    int brightness = -1;
    read_int_bytes(SYSFS_BRIGHTNESS_PATH, &brightness);

    if (brightness) {
        // If we're toggled on (brightness != 0), set it to 0.
        write_int_bytes(SYSFS_BRIGHTNESS_PATH, 0);
        write_int_bytes(HW_BRIGHTNESS_CACHE, brightness);
    } else {
        // Restore hw_brightness cache value.
        int hw_brightness = -1;
        read_int_bytes(HW_BRIGHTNESS_CACHE, &hw_brightness);
        write_int_bytes(SYSFS_BRIGHTNESS_PATH, hw_brightness);
    }

    return 0;
}

int do_color(const char *names[], char *colors[])
{
    int i;
    for (i = 0; i < 4; ++i) {
        if (colors[i] != NULL) {
            char *path = join(SYSFS_COLOR_PREFIX, names[i]);

            FILE *ofs = fs_open(path, "w");

            printf("Setting %s to %s...\n", names[i], colors[i]);

            // Write 6 characters, RRGGBB (in hex).
            fwrite(colors[i], sizeof(char), 6, ofs);
            fs_close(ofs);

            free(path);
        }
    }
    return 0;
}

