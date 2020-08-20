#include "colors.h"
#include "buffer.h"
#include "bytes.h"

char *get_color(const char *path)
{
    char *color = allocate(COLOR_RGB_SIZE + 1);
    read_bytes(path, color, COLOR_RGB_SIZE);
    color[COLOR_RGB_SIZE] = '\0';
    return color;
}
