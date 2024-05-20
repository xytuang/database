#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>

#include "enums.h"
#include "statement.h"
#include "constants.h"

void *rowSlot(Table *table, int rowNum) {
    int pageNum = rowNum / ROWS_PER_PAGE;
    void *page = table->pager->getPage(pageNum);
    int rowOffset = rowNum % ROWS_PER_PAGE;
    int byteOffset = rowOffset * ROW_SIZE;
    return page + byteOffset; 
}

void freeTable(Table *table){
    Pager *pager = table->pager;
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
            return PREPARE_ARGUMENT_SIZE_FAIL; 
        }
        int id = std::stoi(attributes[1]);
        if (id < 0){
            return PREPARE_NEGATIVE_ID;
        }
        statement->id = id;
        
        if (attributes[2].size() > USERNAME_MAX_SIZE){
            return PREPARE_STRING_TOO_LONG;
        }
        statement->username = attributes[2];
        
        if (attributes[3].size() > EMAIL_MAX_SIZE) {
            return PREPARE_STRING_TOO_LONG;
        }
        statement->email = attributes[3];
        return PREPARE_SUCCESS;
    }
    else {
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }
}

int executeSelectStatement(Table *table) {
    Pager *pager = table->pager;
    for (int i = 0; i < pager->numPages; i++){
        Page *page = pager->pages[i];
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
    Pager *pager = table->pager;

    return 1;
}


ExecuteResult executeStatement(Statement *statement, Table *table){
    Pager *pager = table->pager;
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
                    return EXECUTE_SUCCESS;
                }
                else {
                    std::cout << "Insert Failed" << std::endl;
                    return EXECUTE_FAILURE;
                }
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
        switch(prepareStatement(input, &statement)){
            case PREPARE_SUCCESS:
                break;
            case PREPARE_ARGUMENT_SIZE_FAIL:
                std::cout << "usage: <command> <id> <username> <email>" << std::endl;
                continue;
            case PREPARE_NEGATIVE_ID:
                std::cout << "Must be positive ID" << std::endl;
                continue;
            case PREPARE_STRING_TOO_LONG:
                std::cout << "Username or email too long" << std::endl;
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                std::cout << "Unknown command given" << std::endl;
                continue;
        }
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
