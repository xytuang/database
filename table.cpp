#include "table.h"
#include "constants.h"

Table::Table(std::string filename) {
    this->pager = new Pager(filename);    
    this->numRows = this->pager->fileLength / ROW_MAX_SIZE;
}


