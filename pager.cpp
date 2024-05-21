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
    file.seekg(currentPos);
    this->file = &file;
    this->fileLength = fileLength;
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        this->pages[i] = nullptr;
    }
}

Page *Pager::getPage(int pageNum){
    if (pageNum > TABLE_MAX_PAGES) {
        std::cout << "Tried to fetch page number out of bounds." << pageNum << " > " <<  TABLE_MAX_PAGES << std::endl;
        exit(1);
    }
    std::cout << "inside get page" << std::endl;
    if (this->pages[pageNum] == nullptr) {
        Page *page = new Page();
        int numPages = (this->fileLength) / PAGE_MAX_SIZE;
        if (this->fileLength % PAGE_MAX_SIZE) {
            numPages++;
        }
        if (pageNum <= numPages) {
            std::cout << "seeking" << std::endl;
            this->file->seekg(pageNum * PAGE_MAX_SIZE, std::ios::beg);
            std::cout << "seekg" << std::endl;
            this->file->read((char *)page->rows, PAGE_MAX_SIZE);
            std::cout << "read" << std::endl;
            if (this->file->gcount() != PAGE_MAX_SIZE) { //error suspect
                std::cerr << "error reading file" << std::endl;
                exit(1);
            }
        }
        this->pages[pageNum] = page;
    }
    return this->pages[pageNum];
}
//INCOMPLETE?????
void Pager::pagerFlush(int pageNum, int size) {
    if (this->pages[pageNum] == nullptr) {
        std::cerr << "Tried to flush non-existent page" << std::endl;
        exit(1);
    }
    this->file->seekg(pageNum * PAGE_MAX_SIZE, std::ios::beg);
    this->file->write((char *)this->pages[pageNum]->rows, size);
}
