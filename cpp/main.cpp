#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include <stdlib.h>

#include "constants.h"
#include "enums.h"
#include "structs.h"
#include "row.h"
#include "table.h"

std::vector<std::string> splitString(const std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while(std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void serializeRow(Row* source, void* destination) {
    memcpy((int*)destination + ID_OFFSET, &(source->id), ID_SIZE);
    strncpy((char*)destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy((char*)destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserializeRow(void* source, Row* destination) {
    memcpy(&(destination->id), (int*)source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), (char*)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), (char*)source + EMAIL_OFFSET, EMAIL_SIZE);
}

PrepareResult prepareInsert(std::string input, Statement* statement) {
    statement->type = STATEMENT_INSERT;
    std::vector<std::string> argsAssigned = splitString(input, ' ');
    if (argsAssigned.size() != 4) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (argsAssigned[2].size() > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    if (argsAssigned[3].size() > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    int id = std::stoi(argsAssigned[1]);
    
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    statement->rowToInsert.id = id;

    for (int i = 0; i < argsAssigned[2].size(); i++) {
        statement->rowToInsert.username[i] = argsAssigned[2][i];
    }
    statement->rowToInsert.username[argsAssigned[2].size()] = '\0';

    for (int i = 0; i < argsAssigned[3].size(); i++) {
        statement->rowToInsert.email[i] = argsAssigned[3][i];
    }
    statement->rowToInsert.email[argsAssigned[3].size()] = '\0';

    return PREPARE_SUCCESS;
}

PrepareResult prepareStatement(std::string input, Statement* statement) {
    if (input.substr(0,6) == "insert") {
        return prepareInsert(input, statement);
    }
    if (input.substr(0,6) == "select") {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult executeInsert(Statement* statement, Table* table) {
    if (table->numRows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row* rowToInsert = &(statement->rowToInsert);
    serializeRow(rowToInsert, rowSlot(table, table->numRows));
    table->numRows++;
    return EXECUTE_SUCCESS;
}

ExecuteResult executeSelect(Statement* statement, Table* table) {
    Row row;
    for (int i = 0; i < table->numRows; i++) {
        deserializeRow(rowSlot(table, i), &row);
        std::cout << "ID: " << row.id << " USERNAME: " << row.username << " EMAIL: " << row.email << std::endl;
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult executeStatement(Statement* statement, Table* table){
    switch(statement->type) {
        case STATEMENT_INSERT:
            return executeInsert(statement, table);
        case STATEMENT_SELECT:
            return executeSelect(statement, table);
    }
}

MetaCommandResult doMetaCommand(std::string input, Table* table) {
    if (input == ".exit") {
        dbClose(table);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        std:: cout << "Usage: ./main <database file>" << std::endl;
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Table* table = dbOpen(filename);

    std::string input;
    while(true) {
        std::cout << "sqlite> ";
        std::getline(std::cin, input);
        if (input[0] == '.'){
            switch(doMetaCommand(input, table)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    std::cout << "Unrecognized command: " << input << std::endl;
                    continue; 
            }
        }

        Statement statement;
        switch(prepareStatement(input, &statement)){
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                std::cout << "Syntax error Could not parse statement." << std::endl;
                continue;    
            case PREPARE_NEGATIVE_ID:
                std::cout << "ID must be positive." << std::endl;
                continue;
            case PREPARE_STRING_TOO_LONG:
                std::cout << "String too long." << std::endl;
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                std::cout << "Unrecognized keyword at start of " << input << std::endl;
                continue;
        }
        switch(executeStatement(&statement, table)) {
            case EXECUTE_SUCCESS:
                std::cout << "Execute success." << std::endl;
                break;
            case EXECUTE_TABLE_FULL:
                std::cout << "Table is full!" << std::endl;
                break;
        }
        std::cout << "Executed." << std::endl;

    }
    return 0;
}
