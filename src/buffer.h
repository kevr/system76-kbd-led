#ifndef BUFFER_H
#define BUFFER_H

#include <string.h>

//! Allocate a c-string.
char *allocate(size_t size);

//! Strip trailing newlines from a string buffer.
void strip_newlines(char *buffer, const size_t bytes);

#endif /* BUFFER_H */
