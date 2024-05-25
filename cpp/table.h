#ifndef TABLE_H
#define TABLE_H

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "pager.h"
#include "constants.h"

typedef struct {
    Pager* pager;
    uint32_t numRows;
} Table;


void* getPage(Pager* pager, int pageNum) {
    if (pageNum > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", pageNum, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[pageNum] == NULL) {
        void *page = malloc(PAGE_SIZE);
        uint32_t numPages = pager->fileLength / PAGE_SIZE;

        //Handle partial page
        if (pager->fileLength % PAGE_SIZE) {
            numPages++;
        }

        if (pageNum <= numPages) {
            lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
            ssize_t bytesRead = read(pager->fileDescriptor, page, PAGE_SIZE);
            if (bytesRead == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[pageNum] = page;
    }
    return pager->pages[pageNum];


}

void* rowSlot(Table* table, int rowNum){
    int pageNum = rowNum / ROWS_PER_PAGE;
    
    void* page = getPage(table->pager, pageNum);


    int rowOffset = rowNum % ROWS_PER_PAGE;
    int byteOffset = rowOffset * ROW_SIZE;

    char* ptr = static_cast<char*>(page);
    ptr += byteOffset;
    return static_cast<void*>(ptr);
}


Table* dbOpen(const char* filename) {
    Pager* pager = pagerOpen(filename);
    uint32_t numRows = pager->fileLength / ROW_SIZE;

    Table* table = (Table*)malloc(sizeof(Table));

    table->numRows = numRows;
    table->pager = pager;
    return table;
}

void dbClose(Table* table) {
    Pager* pager = table->pager;
    uint32_t numFullPages = table->numRows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < numFullPages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pagerFlush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    //Handle partial page
    uint32_t numAdditionalRows = table->numRows % ROWS_PER_PAGE;
    if (numAdditionalRows > 0) {
        uint32_t pageNum = numFullPages;
        if (pager->pages[pageNum] != NULL) {
            pagerFlush(pager, pageNum, numAdditionalRows * ROW_SIZE);
            free(pager->pages[pageNum]);
            pager->pages[pageNum] = NULL;
        }
    }

    int result = close(pager->fileDescriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

#endif
