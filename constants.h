#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "row.h"

#define sizeOfAttribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

#define TABLE_MAX_PAGES 100
const uint32_t ID_SIZE = sizeOfAttribute(Row, id);
const uint32_t USERNAME_SIZE = sizeOfAttribute(Row, username);
const uint32_t EMAIL_SIZE = sizeOfAttribute(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_SIZE + ID_OFFSET;
const uint32_t EMAIL_OFFSET = USERNAME_SIZE + USERNAME_OFFSET;

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;
#endif
