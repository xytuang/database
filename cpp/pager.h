#ifndef PAGER_H
#define PAGER_H

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "constants.h"

typedef struct {
    int fileDescriptor;
    uint32_t fileLength;
    void* pages[TABLE_MAX_PAGES];
} Pager;

Pager* pagerOpen(const char* filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    off_t fileLength = lseek(fd, 0, SEEK_END);
    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->fileDescriptor = fd;
    pager->fileLength = fileLength;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

void pagerFlush(Pager* pager, uint32_t pageNum, uint32_t size) {
    if (pager->pages[pageNum] == NULL) {
        printf("Tried to flush null pages\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);

    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesWritten = write(pager->fileDescriptor, pager->pages[pageNum], size);
    if (bytesWritten == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
}
#endif
