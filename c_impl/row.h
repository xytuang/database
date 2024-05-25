#ifndef ROW_H
#define ROW_H

#include <stdio.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

void printRow(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
#endif
