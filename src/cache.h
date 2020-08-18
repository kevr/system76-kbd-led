/**
 * @brief File nodes in /var/cache used to maintain persistence.
 *
 * Generally used to cache colors and brightness levels, these functions
 * implement a generic way to read and write file caches.
 *
 * @author Kevin Morris <kevr.gtalk@gmail.com>
 * @license MIT
 **/
#ifndef CACHE_H
#define CACHE_H

#include "colors.h"
#include <string.h>

// Filesystem prefix for program caches.
// This prefix works on Linux only.
#define CACHE_PREFIX "/var/cache/system76-kbd-led/"

#define HW_BRIGHTNESS_CACHE JOIN(CACHE_PREFIX, "hw_brightness")
#define COLORS_CACHE JOIN(CACHE_PREFIX, "colors")

/**
 * @brief Write data to a cache file.
 *
 * @param path Path to output file.
 * @param data Pointer to valid output data.
 * @param len Length of output data.
 **/
void write_cache(const char *path, char *data, const size_t len);

/**
 * @brief Read data from a cache file.
 *
 * @param path Path to output file.
 * @param data Pointer to a valid input buffer.
 * @param len Length of input data.
 **/
void read_cache(const char *path, char *data, const size_t len);
void read_cache_unsafe(const char *path, char *data, const size_t len,
                       char *success);

/**
 * @brief Write an integer to a cache node.
 *
 * @param node Cache node (eg. leaf of /var/cache/system76-kbd-led/brightness).
 * @param value Inteeger value to write.
 **/
void write_int_bytes(const char *path, int value);

/**
 * @brief Reads an integer from a cache node.
 *
 * @param node Cache node (eg. leaf of /var/cache/system76-kbd-led/brightness).
 * @param value An output pointer to integer.
 **/
void read_int_bytes(const char *path, int *value);
void read_int_bytes_unsafe(const char *path, int *value, char *success);

/**
 * @brief Write an array of color pointers into cache.
 *
 * @param colors An array of color pointers.
 **/
void write_color_cache(char *colors[CNT_COLORS]);

/**
 * @brief Read the existing color cache into an array of pointers.
 *
 * @param colors Output array of pointers.
 **/
void read_color_cache(char *colors[CNT_COLORS]);
void read_color_cache_unsafe(char *colors[CNT_COLORS], char *success);

/**
 * @brief Free an array of color pointers.
 *
 * @param colors An array of color pointers.
 **/
void free_color_data(char *colors[CNT_COLORS]);

/**
 * @brief Execute a function on each color pointer in the array.
 *
 * Typically used for `&free` pointer to free up pointer data
 * allocated inside of an array of pointers.
 *
 * @param colors Array of color pointers.
 * @param func Executor function.
 **/
void for_each_color(char *colors[CNT_COLORS], void (*func)(void *));

//! Setup the cache environment. Run this before speaking to the cache.
void setup_cache(void);

#endif /* CACHE_H */
