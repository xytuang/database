#include <iostream>
#include <string>
#include <stdlib.h>


enum StatementType {
    SELECT,
    INSERT
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_FAILURE
};

struct Statement {
    StatementType type;
};

PrepareResult prepareStatement(std::string input, struct Statement *statement){
    if (input.substr(0,6) == "select"){
        statement->type = SELECT;
        return PREPARE_SUCCESS;
    }
    else if (input.substr() == "insert"){
        statement->type = INSERT;
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
                std::cout << "Execute a select statement here" << std::endl;
                break;
        case INSERT:
                std::cout << "Execute a insert statement here" << std::endl;
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

    return 0;
}
