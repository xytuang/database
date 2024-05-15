#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>

#include "enums.h"
#include "structs.h"

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
    if (input == "select"){
        statement->type = SELECT;
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

int executeSelectStatement(Table *table) {
    for (int i = 0; i < table->numPages; i++){
        Page *page = table->pages[i];
        for (int j = 0; j < page->numRows; j++) {
            Row *row = page->rows[j];
            std::cout << "ID: " << row->id << " ";
            std::cout << "username: " << row->username << " ";
            std::cout << "email: " << row->email << std::endl;
        }
    }
    return 1;
}

int executeInsertStatement(Statement *statement, Table *table) {
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
            if (table->numPages == TABLE_MAX_PAGES) {
                return 0;
            }
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
                if(executeSelectStatement(table) == 1) {
                    return EXECUTE_SUCCESS;
                }
                else {
                    return EXECUTE_TABLE_FULL;                    
                }
        case INSERT:
                if(executeInsertStatement(statement, table) == 1){
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
        std::cout << "Should not print" << std::endl;
        switch(executeStatement(&statement, table)) {
            case EXECUTE_SUCCESS:
                std::cout << "Executed" << std::endl;
                break;
            case EXECUTE_TABLE_FULL:
                std::cout << "Table is full!" << std::endl;
                break;
            case EXECUTE_FAILURE:
                std::cout << "Execution failed!" << std::endl;
        }

    }
    freeTable(table);

    return 0;
}
