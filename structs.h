#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <fcntl.h>
#include <unistd.h>

#define TABLE_MAX_PAGES 100
#define PAGE_MAX_ROWS 100
#define EMAIL_MAX_SIZE 255
#define USERNAME_MAX_SIZE 32

struct Row {
    int id;
    std::string username;
    std::string email;
};

struct Statement {
    StatementType type;
    Row row;
};

struct Page {
    int numRows;
    Row *rows[PAGE_MAX_ROWS];
    Page() {
        numRows = 0;
        for (int i = 0; i < PAGE_MAX_ROWS; i++) {
            rows[i] = nullptr;
        }
    }
};

struct Pager {
    int file_descriptor;
    int file_length;
    Page *pages[TABLE_MAX_PAGES];
    Pager(const char *filename) {
        int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd == -1) {
            std::cout << "Unable to open file" << std::endl;
            exit(1);
        }
        file_descriptor = fd;
        file_length = lseek(fd, 0, SEEK_END);
        for (int i = 0; i < TABLE_MAX_PAGES; i++) {
            pages[i] = nullptr;
        }
    }
};

struct Table {
    int numPages;
    Pager *pager;
    Table(const char *filename){
        numPages = 0;
        pager = new Pager(filename);
    }
    Table(){
        numPages = 0;
        pager = new Pager("db.txt");
    }
};

#endif

