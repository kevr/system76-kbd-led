#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_READ_FAILED 2
#define SYSFS_WRITE_FAILED 3

#define SYSFS_PREFIX "/sys/class/leds/system76::kbd_backlight"
#define SYSFS_COLOR_PREFIX SYSFS_PREFIX "/color_"

#define JOIN(prefix, value) prefix value

char *join(const char *prefix, const char *value);

int print_usage(int argc, char *argv[]);
int print_help(int argc, char *argv[]);
int error(int return_code, const char *message);

int main(int argc, char *argv[])
{
    int opt;
    char *left = NULL, *center = NULL, *right = NULL, *extra = NULL;
    char toggle = 0;

    while ((opt = getopt(argc, argv, "hl:c:r:e:t")) != -1) {
        switch (opt) {
        case 'h':
            return print_help(argc, argv);
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
        case 't':
            toggle = 1;
            break;
        case '?':
            return print_usage(argc, argv);
        }
    }

    if (toggle) {
        char buf[5];
        const char *brightness = JOIN(SYSFS_PREFIX, "/brightness");

        FILE *ifs = fopen(brightness, "r");
        if (!ifs)
            return error(SYSFS_READ_FAILED,
                         "unable to open sysfs for reading; are you root?");
        size_t bytes = fread(buf, sizeof(char), 4, ifs);
        fclose(ifs);
        buf[bytes] = '\0';

        // Strip \n from the end of our input by overriding it with \0.
        while (buf[bytes - 1] == '\n')
            buf[--bytes] = '\0';

        if (strcmp(buf, "0") == 0) {
            // Get cached hw_brightness
            char *hw_brightness = JOIN(SYSFS_PREFIX, "/brightness_hw_changed");

            ifs = fopen(hw_brightness, "r");
            if (!ifs)
                return error(
                    SYSFS_READ_FAILED,
                    "unable to open sysfs for reading; are you root?");
            bytes = fread(buf, sizeof(char), 4, ifs);
            fclose(ifs);
            buf[bytes] = '\0';

            // Restore the original brightness.
            ifs = fopen(brightness, "w");
            if (!ifs)
                return error(
                    SYSFS_WRITE_FAILED,
                    "unable to open sysfs for writing; are you root?");
            fwrite(buf, sizeof(char), bytes, ifs);
            fclose(ifs);

        } else {
            // Turn it off; brightness is on!
            ifs = fopen(brightness, "w");
            if (!ifs)
                return error(
                    SYSFS_WRITE_FAILED,
                    "unable to open sysfs for writing; are you root?");
            fwrite("0", sizeof(char), 1, ifs);
            fclose(ifs);
        }
    }

    const char *arr[] = {"left", "center", "right", "extra"};
    char *colors[] = {left, center, right, extra};

    int i;
    for (i = 0; i < 4; ++i) {
        if (colors[i] != NULL) {
            char *p = join(SYSFS_COLOR_PREFIX, arr[i]);
            FILE *ofs = fopen(p, "w");
            if (!ofs) {
                free(p);
                return error(
                    SYSFS_WRITE_FAILED,
                    "unable to open sysfs for writing; are you root?");
            }
            fwrite(colors[i], sizeof(char), 8, ofs);
            fwrite("\n", sizeof(char), 1, ofs);
            fclose(ofs);
            free(p);
        }
    }

    return 0;
}

char *join(const char *prefix, const char *value)
{
    const int prefix_len = strlen(prefix);
    const int value_len = strlen(value);

    char *buf = (char *)malloc(prefix_len + value_len + 1);
    memset(buf, 0, prefix_len + value_len + 1);
    strncpy(buf, prefix, prefix_len);
    strncpy(&buf[prefix_len], value, value_len);

    return buf;
}

int print_usage(int argc, char *argv[])
{
    printf("usage: %s [-hlcret]\n", argv[0]);
    return -1;
}

int print_help(int argc, char *argv[])
{
    print_usage(argc, argv);
    printf("\nProgram options:\n -h\t| Print help.\n -l\t| Left color "
           "(rgb).\n -c\t| Center color (rgb).\n -r\t| Right color (rgb).\n "
           "-e\t| Extra color (rgb).\n -t\t| Toggle keyboard light (on or "
           "off).\n\n");
    return 0;
}

int error(int return_code, const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    return return_code;
}
