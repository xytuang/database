#ifndef PAGER_H
#define PAGER_H

#include <fstream>
#include "constants.h"
#include "structs.h"

class Pager {
public:
    std::fstream *file;
    std::streampos fileLength;
    Page *pages[TABLE_MAX_PAGES];
    Pager(std::string filename);
    Page *getPage(int pageNum);
    void pagerFlush(int pageNum, int size);
};

#endif
