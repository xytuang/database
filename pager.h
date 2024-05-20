#include <fstream>
#include "constants.h"

class Pager {
public:
    std::fstream file;
    std::streampos fileLength;
    void *pages[TABLE_MAX_PAGES];
    Pager(std::string filename);
    void *getPage(int pageNum);
};
