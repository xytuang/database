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

enum ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE
};

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


void freeTable(Table *table){
    for (int i = 0; i < table->numPages; i++){
        Page *page = table->pages[i];
        for (int j = 0; j < page->numRows; j++) {
            delete page->rows[j];
        }
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

PrepareResult prepareStatement(std::string input, Statement *statement){
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
    else if (input.substr(0,6) == "insert"){
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

int insertStatement(Statement *statement, Table *table) {
    int numPages = table->numPages;
    Page *page;
    if (numPages == 0) { 
        page = new Page();
        table->pages[numPages] = page;
        table->numPages++;
    }
    else {
        page = table->pages[numPages - 1];
        if (page->numRows == 100) { 
            page = new Page();
            table->pages[numPages] = page;
            table->numPages++;
        }
    }

    page->rows[page->numRows] = new Row;
    page->rows[page->numRows]->id = statement->row.id;
    page->rows[page->numRows]->username = statement->row.username;
    page->rows[page->numRows]->email = statement->row.email;
    page->numRows++;
    return 1;
}


ExecuteResult executeStatement(Statement *statement, Table *table){
    switch(statement->type) {
        case SELECT:
                std::cout << "ID: " <<  statement->row.id << std::endl;
                std::cout << "username: " <<  statement->row.username << std::endl;
                std::cout << "email: " << statement->row.email << std::endl;
                return EXECUTE_SUCCESS;
        case INSERT:
                if(insertStatement(statement, table) == 1){
                    std::cout << "Insert Success" << std::endl;

                    //std::cout << "Number of pages: " << table->numPages << std::endl;

                    Page *insertedPage = table->pages[table->numPages - 1]; 

                    //std::cout << "Latest row: " << insertedPage->numRows - 1 << std::endl;

                    Row *insertedRow = insertedPage->rows[insertedPage->numRows - 1];

                    std::cout << "ID: " << insertedRow->id << std::endl;
                    std::cout << "username: " << insertedRow->username << std::endl;
                    std::cout << "email: " << insertedRow->email << std::endl;
                }
                else {
                    std::cout << "insert failed" << std::endl;
                }
                return EXECUTE_SUCCESS;
        default:
                std::cout << "Execute fail" << std::endl;
                return EXECUTE_FAILURE;
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
    std::cout << std::endl;

    Table *table = new Table();

    std::string input;
    while(true) {
        std::cout << "sqlite> ";
        std::getline(std::cin, input);
        if (input[0] == '.'){
            doMetaCommand(input);
        }

        Statement statement;
        if (prepareStatement(input, &statement) == PREPARE_FAILURE){
            std::cout << "Prepare Failure" << std::endl;
            continue;
        }

        executeStatement(&statement, table);

        std::cout << "Executed" << std::endl;
    }
    freeTable(table);

    return 0;
}
