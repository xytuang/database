#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>


#define TABLE_MAX_PAGES 100
#define PAGE_MAX_ROWS 100

enum StatementType {
    SELECT,
    INSERT
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_FAILURE
};

struct Row {
    int id;
    std::string username;
    std::string email;
};

struct Statement {
    StatementType type;
    struct Row row;
};

struct Page {
    int numRows;
    struct Row *rows[PAGE_MAX_ROWS];
};

struct Table {
    int numPages;
    struct Page *pages[TABLE_MAX_PAGES];
};

struct Table *allocateTable() {
    struct Table *table = new Table;
    table->numPages = 0;
    return table;
};

void freeTable(struct Table *table){
    for (int i = 0; i < table->numPages; i++){
        delete table->pages[i];
    }
    delete table;
}

std::vector<std::string> splitString(const std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while(std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

PrepareResult prepareStatement(std::string input, struct Statement *statement){
    if (input.substr(0,6) == "select"){
        statement->type = SELECT;
        std::vector<std::string> attributes = splitString(input, ' ');
        if (attributes.size() != 4) {
            return PREPARE_FAILURE;
        }
        statement->row.id = std::stoi(attributes[1]);
        statement->row.username = attributes[2];
        statement->row.email = attributes[3];
        return PREPARE_SUCCESS;
    }
    else if (input.substr() == "insert"){
        statement->type = INSERT;
        std::vector<std::string> attributes = splitString(input, ' ');
        if (attributes.size() != 4) {
            return PREPARE_FAILURE;
        }
        statement->row.id = std::stoi(attributes[1]);
        statement->row.username = attributes[2];
        statement->row.email = attributes[3];
        return PREPARE_SUCCESS;
    }
    else {
        std::cout << "Unknown command: " << input << std::endl;
        return PREPARE_FAILURE;
    }
}

void executeStatement(std::string input, struct Statement *statement){
    switch(statement->type) {
        case SELECT:
                std::cout << "ID: " <<  statement->row.id << std::endl;
                std::cout << "username: " <<  statement->row.username << std::endl;
                std::cout << "email: " << statement->row.email << std::endl;
                break;
        case INSERT:
                std::cout << "ID: " << statement->row.id << std::endl;
                std::cout << "username: " << statement->row.username << std::endl;
                std::cout << "email: " << statement->row.email << std::endl;
                break;
    }
}

void doMetaCommand(std::string input){
    if (input == ".exit"){
        std::cout << "Exiting" << std::endl;
        exit(0);
    }
    else {
        std::cout << "Do a bunch of meta commands here" << std::endl;
    }
}

int main(){
    std::cout << "Welcome to Bootleg sqlite!" << std::endl;
    std::cout << "Enter \".help\" for usage hints." << std::endl;
    std::cout << "Connected to a transient in-memory database." << std::endl;
    std::cout << "Use \".open FILENAME\" to reopen on a persistent database." << std::endl;

    struct Table *table = allocateTable();

    std::string input;
    while(true) {
        std::cout << "sqlite> ";
        std::getline(std::cin, input);
        if (input[0] == '.'){
            doMetaCommand(input);
        }

        struct Statement statement;
        if (prepareStatement(input, &statement) == PREPARE_FAILURE){
            continue;
        }

        executeStatement(input, &statement);

        std::cout << "Executed" << std::endl;
    }
    freeTable(table);

    return 0;
}
