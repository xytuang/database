#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <fcntl.h>
#include <unistd.h>

#define TABLE_MAX_PAGES 100
#define PAGE_MAX_SIZE 4096
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
    std::vector<Row *>rows;
    char *data;
    int remainingSize;
    Page() {
        numRows = 0;
        data = (char *)malloc(PAGE_MAX_SIZE);
        remainingSize = PAGE_MAX_SIZE;
    }
};

struct Pager {
    int fileDescriptor;
    int fileLength;
    int numPages;
    Page *pages[TABLE_MAX_PAGES];
    Pager(const char *filename) {
        int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd == -1) {
            std::cout << "Unable to open file" << std::endl;
            exit(1);
        }
        fileDescriptor = fd;
        fileLength = lseek(fd, 0, SEEK_END);
        for (int i = 0; i < TABLE_MAX_PAGES; i++) {
            pages[i] = nullptr;
        }
        numPages = 0;
    }
};

struct Table {
    Pager *pager;
    int numRows;
    Table(const char *filename){
        numRows = 0;
        pager = new Pager(filename);
    }
    Table(){
        numRows = 0;
        pager = new Pager("db.txt");
    }
};

#endif

