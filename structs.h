#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include "constants.h"
#include "enums.h"

struct Statement {
    StatementType type;
    int id;
    std::string username;
    std::string email;
};

struct Row {
    int id;
    std::string username;
    std::string email;
};

struct Page {
    Row *rows[ROWS_PER_PAGE];
    Page() {
        for (int i = 0; i < ROWS_PER_PAGE; i++) {
            rows[i] = nullptr;
        }
    }
};


#endif

