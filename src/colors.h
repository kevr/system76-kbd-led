#ifndef COLORS_H
#define COLORS_H

#include "fs.h"

// 1 Left, 2 Center, 3 Right, 4 Extra.
#define CNT_COLORS 4

#define COLOR_IDX_SIZE sizeof(size_t)   // Color index size.
#define COLOR_RGB_SIZE sizeof(char) * 6 // Color value size.

#define SYSFS_COLOR_LEFT JOIN(SYSFS_COLOR_PREFIX, "left")
#define SYSFS_COLOR_CENTER JOIN(SYSFS_COLOR_PREFIX, "center")
#define SYSFS_COLOR_RIGHT JOIN(SYSFS_COLOR_PREFIX, "right")
#define SYSFS_COLOR_EXTRA JOIN(SYSFS_COLOR_PREFIX, "extra")

/**
 * @brief Get a color node's value by it's path.
 *
 * A valid path includes one of SYSFS_COLOR_
 *
 * @param path Color node path.
 * @returns Dynamically allocated c-string that should be freed by the caller.
 **/
char *get_color(const char *path);

#endif /* COLORS_H */
