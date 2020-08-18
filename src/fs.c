#include "fs.h"
#include "buffer.h"
#include "error.h"
#include <stdlib.h>
#include <unistd.h>

char *join(const char *prefix, const char *suffix)
{
    const int prefix_len = strlen(prefix);
    const int suffix_len = strlen(suffix);

    char *buf = allocate(prefix_len + suffix_len + 1);
    memcpy(buf, prefix, prefix_len);
    memcpy(&buf[prefix_len], suffix, suffix_len);

    return buf;
}

int fs_exists(const char *path)
{
    return access(path, F_OK) != -1 ? 1 : 0;
}

FILE *fs_open(const char *path, const char *modes)
{
    FILE *f = fopen(path, modes);
    if (!f) {
        fprintf(stderr, "error: unable to open %s (%s).\n", path, modes);
        exit(FS_OPEN_FAILED);
    }
    return f;
}

int fs_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

void fs_close(FILE *f)
{
    fclose(f);
}
