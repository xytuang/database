#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"
#include "row.h"

typedef struct {
    StatementType type;
    Row rowToInsert;
} Statement;

#endif
