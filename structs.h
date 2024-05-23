#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"
#include "row.h"
#include "constants.h"

typedef struct {
    char* buffer;
    size_t bufferLength;
    ssize_t inputLength;
} InputBuffer;


typedef struct {
    StatementType type;
    Row rowToInsert;
} Statement;


#endif
