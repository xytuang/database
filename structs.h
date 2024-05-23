#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"

typedef struct {
    char* buffer;
    size_t bufferLength;
    ssize_t inputLength;
} InputBuffer;

typedef struct {
    StatementType type;
} Statement;

#endif
