#include "buffer.h"
#include <stdlib.h>

char *allocate(size_t size)
{
    char *ptr = (char *)malloc(sizeof(char) * size);
    memset(ptr, 0, sizeof(char) * size);
    return ptr;
}

void strip_newlines(char *buffer, size_t bytes)
{
    while (buffer[bytes] == '\n')
        buffer[bytes--] = '\0';
}

