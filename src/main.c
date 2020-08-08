#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_PREFIX "/sys/class/leds/system76::kbd_backlight/color_"

char *sysfs_path(const char *pos);
int print_usage(int argc, char *argv[]);
int print_help(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int opt;
    char *left = NULL, *center = NULL, *right = NULL, *extra = NULL;

    while ((opt = getopt(argc, argv, "hl:c:r:e:")) != -1) {
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
        case '?':
            return print_usage(argc, argv);
        }
    }

    const char *arr[] = {"left", "center", "right", "extra"};
    char *colors[] = {left, center, right, extra};

    int i;
    for (i = 0; i < 4; ++i) {
        if (colors[i] != NULL) {
            char *p = sysfs_path(arr[i]);
            FILE *ofs = fopen(p, "w");
            fwrite(colors[i], sizeof(char), 8, ofs);
            fwrite("\n", sizeof(char), 1, ofs);
            fclose(ofs);
            free(p);
        }
    }

    return 0;
}

char *sysfs_path(const char *pos)
{
    const int prefix_len = strlen(SYSFS_PREFIX);
    const int pos_len = strlen(pos);

    char *buf = (char *)malloc(prefix_len + pos_len + 1);
    memset(buf, 0, prefix_len + pos_len + 1);
    strncpy(buf, SYSFS_PREFIX, prefix_len);
    strncpy(&buf[prefix_len], pos, pos_len);
    return buf;
}

int print_usage(int argc, char *argv[])
{
    printf("usage: %s [-hlcre]\n", argv[0]);
    return -1;
}

int print_help(int argc, char *argv[])
{
    print_usage(argc, argv);
    printf("\nProgram options:\n -h\t| Print help.\n -l\t| Left color "
           "(rgb).\n -c\t| Center color (rgb).\n -r\t| Right color (rgb).\n "
           "-e\t| Extra color (rgb).\n\n");
    return 0;
}

