#ifndef TABLE_H
#define TABLE_H
#include <stdlib.h>
#include "constants.h"

typedef struct {
    uint32_t numRows;
    void* pages[TABLE_MAX_PAGES];
} Table;

Table* newTable() {
    Table* table = (Table *) malloc(sizeof(Table));
    table->numRows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

void freeTable(Table *table){
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

#endif
