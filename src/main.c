#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SYSFS_OPEN_FAILED 2
#define INVALID_ARGUMENT 1

#define SYSFS_PREFIX "/sys/class/leds/system76::kbd_backlight/"
#define SYSFS_COLOR_PREFIX SYSFS_PREFIX "color_"

#define CACHE_PATH "/var/cache/system76-kbd-led/brightness"
#define HW_CACHE_PATH "/var/cache/system76-kbd-led/hw_brightness"

#define JOIN(prefix, value) prefix value

// Bounds for checking.
#define DEFAULT_INT_LENGTH 3

char *join(const char *prefix, const char *suffix);

int print_usage(int argc, char *argv[]);
int print_help(int argc, char *argv[]);
int error(int return_code, const char *message);

void prepare_cache(void);

FILE *open_file(const char *path, const char *modes);
int filesize(FILE *f);
char *dirname(const char *path);

char *allocate(int size);
void strip_newlines(char *buffer, const size_t bytes);

int main(int argc, char *argv[])
{
    int opt;
    char *left = NULL, *center = NULL, *right = NULL, *extra = NULL;
    char toggle = 0;
    int brightness_change = 0;

    while ((opt = getopt(argc, argv, "htl:c:r:e:b:")) != -1) {
        switch (opt) {
        case 'h':
            return print_help(argc, argv);
        case 't':
            toggle = 1;
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
            brightness_change = strtol(optarg, NULL, 10);
            if (errno)
                return error(errno, strerror(errno));
            break;
        case '?':
            return print_usage(argc, argv);
        }
    }

    // If -b (brightness) was given.
    if (brightness_change != 0) {
        const char *brightness = JOIN(SYSFS_PREFIX, "brightness");
        const char *max_brightness = JOIN(SYSFS_PREFIX, "max_brightness");

        FILE *ifs = open_file(brightness, "r");
        int size = filesize(ifs);
        char *buf = allocate(size + 1);
        size_t bytes = fread(buf, sizeof(char), size, ifs);
        buf[bytes] = '\0';
        fclose(ifs);
        int current = strtol(buf, NULL, 10);

        prepare_cache();
        ifs = open_file(max_brightness, "r");
        size = filesize(ifs);
        bytes = fread(buf, sizeof(char), size, ifs);
        buf[bytes] = '\0';
        fclose(ifs);
        int max = strtol(buf, NULL, 10);

        const int LOWER_BOUND = 45;
        const int UPPER_BOUND = max;

        int new_value = current + brightness_change;
        if (new_value < LOWER_BOUND)
            new_value = LOWER_BOUND;
        else if (new_value > UPPER_BOUND)
            new_value = UPPER_BOUND;

        // Take log10(N) + 1 as the possible length of the value integer.
        int len = log10(new_value) + 1;
        char *output = allocate(len + 1);
        sprintf(output, "%d", new_value);

        ifs = open_file(brightness, "w");
        fwrite(output, sizeof(char), strlen(output), ifs);
        fclose(ifs);

        FILE *ofs = open_file(CACHE_PATH, "w");
        fwrite(output, sizeof(char), strlen(output), ofs);
        fclose(ofs);

        free(output);
        free(buf);
    }

    prepare_cache();

    // If -t (toggle) was given.
    if (toggle) {
        const char *brightness = JOIN(SYSFS_PREFIX, "brightness");

        // Read current brightness value.
        FILE *ifs = open_file(brightness, "r");
        int size = filesize(ifs);
        char *buf = allocate(size + 1);
        size_t bytes = fread(buf, sizeof(char), 4, ifs);
        fclose(ifs);

        // Prepare buffer.
        buf[bytes] = '\0';
        strip_newlines(buf, bytes - 1);

        if (strcmp(buf, "0") == 0) {
            // Get cached hw_brightness
            const char *hw_brightness =
                JOIN(SYSFS_PREFIX, "brightness_hw_changed");

            ifs = open_file(CACHE_PATH, "r");
            size = filesize(ifs);

            char *cache_buf = allocate(size + 1);
            bytes = fread(cache_buf, sizeof(char), size, ifs);
            fclose(ifs);

            cache_buf[bytes] = '\0';
            strip_newlines(cache_buf, bytes - 1);

            // Restore the original brightness.
            ifs = open_file(brightness, "w");
            fwrite(cache_buf, sizeof(char), bytes, ifs);
            fclose(ifs);

            free(cache_buf);

        } else {
            ifs = open_file(brightness, "r");
            size = filesize(ifs);
            char *brightness_value = allocate(size + 1);
            bytes = fread(brightness_value, sizeof(char), size, ifs);
            fclose(ifs);

            brightness_value[bytes] = '\0';
            strip_newlines(brightness_value, bytes - 1);

            // Save brightness value in the cache.
            ifs = open_file(CACHE_PATH, "w");
            fwrite(brightness_value, sizeof(char), size, ifs);
            fclose(ifs);

            // Turn it off!
            ifs = open_file(brightness, "w");
            fwrite("0", sizeof(char), 1, ifs);
            fclose(ifs);

            free(brightness_value);
        }

        free(buf);
    }

    const char *arr[] = {"left", "center", "right", "extra"};
    char *colors[] = {left, center, right, extra};

    int i;
    for (i = 0; i < 4; ++i) {
        if (colors[i] != NULL) {
            char *path = join(SYSFS_COLOR_PREFIX, arr[i]);
            FILE *ofs = open_file(path, "w");
            fwrite(colors[i], sizeof(char), 8, ofs);
            fwrite("\n", sizeof(char), 1, ofs);
            fclose(ofs);
            free(path);
        }
    }

    return 0;
}

char *join(const char *prefix, const char *suffix)
{
    const int prefix_len = strlen(prefix);
    const int suffix_len = strlen(suffix);

    char *buf = allocate(prefix_len + suffix_len + 1);
    memcpy(buf, prefix, prefix_len);
    memcpy(&buf[prefix_len], suffix, suffix_len);

    return buf;
}

int print_usage(int argc, char *argv[])
{
    printf("usage: %s [-hlcretb]\n", argv[0]);
    return -1;
}

int print_help(int argc, char *argv[])
{
    print_usage(argc, argv);
    printf(
        "\nProgram options:\n -h\t\t| Print help.\n -l <arg>\t| Left color "
        "(rgb).\n -c <arg>\t| Center color (rgb).\n -r <arg>\t| Right color "
        "(rgb).\n "
        "-e <arg>\t| Extra color (rgb).\n -t\t\t| Toggle keyboard light (on "
        "or "
        "off).\n -b <arg>\t| A negative or positive inc/dec value.\n\n");
    return 0;
}

int error(int return_code, const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    return return_code;
}

// Setup cache.
void prepare_cache(void)
{
    char is_valid = 1;
    if (access(CACHE_PATH, F_OK) == -1)
        is_valid = 0;

    // Create a copy of CACHE_PATH.
    char *buf = allocate(strlen(CACHE_PATH) + 1);
    strcpy(buf, CACHE_PATH);

    // Pass it to dirname to get the directory.
    char *dir = dirname(buf);
    free(buf);

    // Try to mkdir.
    int rc = mkdir(dir, 0777);

    // We care about errno's other than EEXIST.
    if (rc == -1 && errno != EEXIST) {
        fprintf(stderr, "error: unable to create cache directory at %s.", dir);
        abort();
    }

    const char *brightness_path = JOIN(SYSFS_PREFIX, "brightness_hw_changed");

    FILE *ifs = fopen(HW_CACHE_PATH, "r");
    if (!ifs) {
        ifs = open_file(brightness_path, "r");
        int size = filesize(ifs);
        char *current = allocate(size + 1);
        int bytes = fread(current, sizeof(char), size, ifs);
        fclose(ifs);
        current[bytes] = '\0';

        FILE *ofs = open_file(HW_CACHE_PATH, "w");
        fwrite(current, sizeof(char), bytes, ofs);
        fclose(ofs);

        ofs = open_file(CACHE_PATH, "w");
        fwrite(current, sizeof(char), bytes, ofs);
        fclose(ofs);

        free(current);
    } else {
        int size = filesize(ifs);
        char *current = allocate(size + 1);
        int bytes = fread(current, sizeof(char), size, ifs);
        fclose(ifs);
        current[bytes] = '\0';

        FILE *ofs = open_file(brightness_path, "r");
        size = filesize(ofs);
        char *sys_current = allocate(size + 1);
        bytes = fread(sys_current, sizeof(char), size, ofs);
        fclose(ofs);
        sys_current[bytes] = '\0';

        // If the values are different, update the cache!
        if (strcmp(current, sys_current) != 0) {
            ofs = open_file(CACHE_PATH, "w");
            fwrite(sys_current, sizeof(char), strlen(sys_current), ofs);
            fclose(ofs);
        }

        free(current);
        free(sys_current);
    }

    // Save initial brightness value to CACHE_PATH.
    if (!is_valid) {
        FILE *ifs = open_file(brightness_path, "r");
        int size = filesize(ifs);
        char *current = allocate(size + 1);
        size_t bytes = fread(current, sizeof(char), size, ifs);
        fclose(ifs);

        current[bytes] = '\0';
        strip_newlines(current, bytes - 1);

        ifs = open_file(CACHE_PATH, "w");
        fwrite(current, sizeof(char), bytes, ifs);
        fclose(ifs);

        free(current);
    }

    // Free the buffer.
    free(dir);
}

FILE *open_file(const char *path, const char *modes)
{
    FILE *f = fopen(path, modes);
    if (!f) {
        fprintf(stderr, "error: unable to open %s (%s).\n", path, modes);
        exit(SYSFS_OPEN_FAILED);
    }
    return f;
}

int filesize(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

char *allocate(int size)
{
    char *buf = (char *)malloc(sizeof(char) * size);
    memset(buf, 0, size);
    return buf;
}

void strip_newlines(char *buffer, size_t bytes)
{
    while (buffer[bytes] == '\n')
        buffer[bytes--] = '\0';
}

char *dirname(const char *path)
{
    // Get position of the last character before /
    int i = strlen(path) - 1;
    while (path[i] != '/')
        --i;

    // Allocate a new buffer the length of the full dir path.
    char *buf = allocate(i + 1);

    // Copy the dir portion of the path.
    strncpy(buf, path, i);
    buf[i] = '\0';

    return buf;
}
