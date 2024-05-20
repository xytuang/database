#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>



struct Statement {
    StatementType type;
    int id;
    std::string username;
    std::string email;
};




#endif

