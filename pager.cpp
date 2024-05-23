#include "pager.h"
#include <iostream>
#include "constants.h"
#include "structs.h"

Pager::Pager(std::string filename) {
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
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
    if (this->pages[pageNum] == nullptr) {
        Page *page = new Page(); 
        this->pages[pageNum] = page;
    }
    return this->pages[pageNum];
}

void Pager::pagerFlush(int pageNum, int size) {
    if (this->pages[pageNum] == nullptr) {
        std::cerr << "Tried to flush non-existent page" << std::endl;
        exit(1);
    }
    
    //std::streampos seekPos = pageNum * PAGE_MAX_SIZE;

    // Check if the stream is in a good state
    //if (!this->file->good()) {
    //    std::cerr << "File stream is not in a good state" << std::endl;
    //    return;
    //}


    //this->file->seekg(seekPos, std::ios::beg);
    
    //if (this->file->fail()) {
    //    std::cerr << "Failed to seek to position: " << seekPos << std::endl;
    //} else {
    //    std::cout << "Successfully seeked to position: " << seekPos << std::endl;
    //}

    for (int i = 0; i < ROWS_PER_PAGE; i++){
        if (this->pages[pageNum]->rows[i] == nullptr){
            continue;
        }
        Row *row = this->pages[pageNum]->rows[i];
        std::cout << "id: " << row->id << std::endl; 
        std::cout << "username: " << row->username << std::endl;
        std::cout << "email: " << row->email << std::endl;
        this->file->write(reinterpret_cast<const char*>(&(row->id)), sizeof(row->id));
        this->file->write(row->username.c_str(), row->username.size());
        this->file->write(row->email.c_str(), row->email.size());
        //this->file << row->id << ',' << row->username << ',' << row->id << std::endl;
        //this->file->write((char *)(this->pages[pageNum]->rows[i]), ROW_MAX_SIZE);
    }
}
