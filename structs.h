
#ifndef STRUCTS_H
#define STRUCTS_H

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

struct Table {
    int numPages;
    Page *pages[TABLE_MAX_PAGES];
    Table(){
        numPages = 0;
        for (int i = 0; i < TABLE_MAX_PAGES; i++){
            pages[i] = nullptr;
        }
    }
};

#endif

