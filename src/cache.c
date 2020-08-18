#include "cache.h"
#include "buffer.h"
#include "error.h"
#include "fs.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

void write_cache(const char *path, char *data, const size_t len)
{
    FILE *ofs = fs_open(path, "w");

    size_t bytes = fwrite(data, sizeof(char), len, ofs);
    if (bytes != len) {
        fprintf(stderr, "error: failed to write cache %s.\n", path);
        exit(FS_WRITE_FAILED);
    }

    fs_close(ofs);
}

void read_cache(const char *path, char *data, const size_t len)
{
    FILE *ifs = fs_open(path, "r");

    size_t bytes = fread(data, sizeof(char), len, ifs);
    if (bytes == 0) {
        fprintf(stderr, "error: failed to read cache %s.\n", path);
        exit(FS_READ_FAILED);
    }

    // Null terminate it.
    data[bytes] = '\0';

    fs_close(ifs);
}

void read_cache_unsafe(const char *path, char *data, const size_t len,
                       char *success)
{
    FILE *ifs = fopen(path, "r");
    if (!ifs) {
        *success = 0;
        return;
    }

    size_t bytes = fread(data, sizeof(char), len, ifs);
    if (bytes == 0) {
        *success = 0;
        fs_close(ifs);
        return;
    }

    // Null terminate it.
    data[bytes] = '\0';

    fs_close(ifs);

    *success = 1;
}

void write_int_bytes(const char *path, int value)
{
    // Longest possible string length for value.
    const size_t len = value > 0 ? log10(value) + 1 : 2;

    char *bytes = allocate(len + 2);
    snprintf(bytes, len + 1, "%d", value);
    printf("value: %d, bytes: %s\n", value, bytes);

    // Write it out to the cache file as bytes.
    write_cache(path, bytes, len);

    free(bytes);
}

void read_int_bytes(const char *path, int *value)
{
    // Longest possible string length for int.
    const size_t len = log10(INT_MAX) + 1;

    char *buf = allocate(len + 1);

    // Read it from the cache file as bytes.
    read_cache(path, buf, len);

    // Convert the bytes we read into an integer.
    *value = strtol(buf, NULL, 10);

    free(buf);
}

void read_int_bytes_unsafe(const char *path, int *value, char *success)
{
    // Longest possible string length for int.
    const size_t len = log10(INT_MAX) + 1;

    char *buf = allocate(len + 1);

    // Read it from the cache file as bytes.
    read_cache_unsafe(path, buf, len, success);

    // Convert the bytes we read into an integer.
    *value = strtol(buf, NULL, 10);

    free(buf);
}

void write_color_cache(char *colors[CNT_COLORS])
{
    // When writing, we layout the colors in the form:
    // [color_index][red][green][blue]
    // [4 byte unsigned integer][2 byte r][2 byte g][2 byte b]
    // s
    // This will allow us to write and read arbitrary
    // configurations with a dynamically sized file.

    // Collect a count of colors so we can allocate the corrent
    // amount for our output buffer.
    size_t count = 0;
    size_t i;
    for (i = 0; i < CNT_COLORS; ++i)
        if (colors[i] != NULL)
            ++count;

    const size_t buffer_size = COLOR_RGB_SIZE * CNT_COLORS;

    // Allocate an output buffer.
    char *output = allocate(buffer_size);

    // Prepare the output buffer.
    size_t k;
    for (i = 0, k = 0; i < CNT_COLORS; ++i) {
        memcpy(&output[k++ * COLOR_RGB_SIZE], colors[i], COLOR_RGB_SIZE);
    }

    // Write our assembled output buffer to cache.
    write_cache(JOIN(CACHE_PREFIX, "colors"), output, buffer_size);
    free(output);
}

void read_color_cache(char *colors[CNT_COLORS])
{
    // When reading the cache, we use lower level file reading
    // functions to get a rolling dynamic color cache that
    // was written with write_color_cache.
    FILE *ifs = fs_open(JOIN(CACHE_PREFIX, "colors"), "r");

    size_t count = 0;
    size_t i;

    // Set all elements to NULL first; Expect that colors is an array of
    // pointers that are not yet allocated.
    for (i = 0; i < CNT_COLORS; ++i)
        colors[i] = NULL;

    for (i = 0; i < CNT_COLORS; ++i) {
        // Allocate COLOR_RGB_SIZE + 1 for \0
        colors[i] = allocate(COLOR_RGB_SIZE + 1);

        // Read the 6 byte color from the file into a null terminated str.
        size_t bytes = fread(colors[i], sizeof(char), COLOR_RGB_SIZE, ifs);

        // Otherwise, set the COLOR_IDX_SIZE + COLOR_RGB_SIZE element to '\0'
        // to finish.
        colors[i][bytes] = '\0';
    }

    fs_close(ifs);
}

void read_color_cache_unsafe(char *colors[CNT_COLORS], char *success)
{
    // When reading the cache, we use lower level file reading
    // functions to get a rolling dynamic color cache that
    // was written with write_color_cache.
    FILE *ifs = fopen(JOIN(CACHE_PREFIX, "colors"), "r");
    if (!ifs) {
        *success = 0;
        return;
    }

    size_t count = 0;
    size_t i;

    // Set all elements to NULL first; Expect that colors is an array of
    // pointers that are not yet allocated.
    for (i = 0; i < CNT_COLORS; ++i)
        colors[i] = NULL;

    for (i = 0; i < CNT_COLORS; ++i) {
        // Allocate COLOR_RGB_SIZE + 1 for \0
        colors[i] = allocate(COLOR_RGB_SIZE + 1);

        // Read the 6 byte color from the file into a null terminated str.
        size_t bytes = fread(colors[i], sizeof(char), COLOR_RGB_SIZE, ifs);

        // Otherwise, set the COLOR_IDX_SIZE + COLOR_RGB_SIZE element to '\0'
        // to finish.
        colors[i][bytes] = '\0';
    }

    fs_close(ifs);
    *success = 1;
}
void free_color_data(char *colors[CNT_COLORS])
{
    for_each_color(colors, &free);
}

void for_each_color(char *colors[CNT_COLORS], void (*func)(void *))
{
    size_t i;
    for (i = 0; i < CNT_COLORS; ++i)
        func(colors[i]);
}

void setup_cache(void)
{
    // Update hw_brightness.
    if (mkdir(CACHE_PREFIX, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "error: unable to mkdir %s\n", CACHE_PREFIX);
        exit(CACHE_MKDIR_FAILED);
    }

    char success = 0;

    // Create hw_brightness cache if it doesn't exist with
    // /sys/class/leds/system76::kbd_backlight/brightness_hw_changed.
    if (!fs_exists(HW_BRIGHTNESS_CACHE)) {
        int hw_brightness = -1;
        read_int_bytes_unsafe(SYSFS_HW_BRIGHTNESS_PATH, &hw_brightness,
                              &success);
        if (!success)
            read_int_bytes(SYSFS_BRIGHTNESS_PATH, &hw_brightness);
        write_int_bytes(HW_BRIGHTNESS_CACHE, hw_brightness);
    }
}
