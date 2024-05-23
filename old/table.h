#include "pager.h"

class Table {
    public:
        Pager *pager;
        int numRows;
        Table(std::string filename);
};
