#ifndef TABLE_H
#define TABLE_H

#include <cstdlib>
#include <iostream>

#include "constants.h"

typedef struct {
    int numRows;
    void* pages[TABLE_MAX_PAGES];
} Table;

void* rowSlot(Table* table, int rowNum){
    int pageNum = rowNum / ROWS_PER_PAGE;
    void* page = table->pages[pageNum];
    if (page == nullptr) {
        page = new char[PAGE_SIZE];
        table->pages[pageNum] = page;
    }
    int rowOffset = rowNum % ROWS_PER_PAGE;
    int byteOffset = rowOffset * ROW_SIZE;

    char* ptr = static_cast<char*>(page);
    ptr += byteOffset;
    return static_cast<void*>(ptr);
}


Table* newTable() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->numRows = 0;
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = nullptr;
    }
    return table;
}

void freeTable(Table* table) {
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

#endif
