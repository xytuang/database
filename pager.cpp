#include "pager.h"
#include <iostream>

Pager::Pager(std::string filename) {
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
    std::streampos currentPos = file.tellg();

    // Seek to the end of the file
    file.seekg(0, std::ios::end);

    // Get the position of the end of the file
    std::streampos fileLength = file.tellg();

    // Restore the file pointer to the original position
    dbFile.seekg(currentPos);
    this->file = dbFile;
    this->fileLength = fileLength;
}

void * Pager::getPage(int pageNum){
    if (page_num > TABLE_MAX_PAGES) {
        std::cout << "Tried to fetch page number out of bounds." << page_num << " > " <<  TABLE_MAX_PAGES << std::endl;
        exit(1);
    }
    if (this->pages[pageNum] == nullptr) {
        void *page = malloc(PAGE_MAX_SIZE);
        int numPages = (this->fileLength) / PAGE_MAX_SIZE;
        if (this->fileLength % PAGE_MAX_SIZE) {
            numPages++;
        }
        if (pageNum <= numPages) {
            this->file.seekg(pageNum * PAGE_MAX_SIZE, std::ios::beg);
            this->file.read(page, PAGE_MAX_SIZE);
            if (this->file.gcount() != PAGE_MAX_SIZE) { //error suspect
                std::cerr << "error reading file" << std::endl;
                exit(1);
            }
        }
        this->pages[pageNum] = page;
    }
    return this->pages[pageNum];
}
