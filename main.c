#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "enums.h"
#include "structs.h"

InputBuffer* newInputBuffer() {
    InputBuffer* inputBuffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    inputBuffer->buffer = NULL;
    inputBuffer->bufferLength = 0;
    inputBuffer->inputLength = 0;

    return inputBuffer;
}

void printPrompt() {printf("db > ");}

void readInput(InputBuffer* inputBuffer) {
    ssize_t bytesRead = getline(&(inputBuffer->buffer), &(inputBuffer->bufferLength), stdin);
    if (bytesRead <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    //ignore newline character at end
    inputBuffer->inputLength = bytesRead - 1;
    inputBuffer->buffer[bytesRead - 1] = 0;
}


PrepareResult prepareStatement(InputBuffer* inputBuffer, Statement* statement) {
    if (strncmp(inputBuffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if (strcmp(inputBuffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void executeStatement(Statement* statement) {
    switch(statement->type){
        case (STATEMENT_INSERT):
            printf("This is where we insert.\n");
            break;
        case (STATEMENT_SELECT):
            printf("This is where we select.\n");
            break;
    }
}
void closeInputBuffer(InputBuffer* inputBuffer) {
    free(inputBuffer->buffer);
    free(inputBuffer);
}

MetaCommandResult doMetaCommand(InputBuffer* inputBuffer) {
    if (strcmp(inputBuffer->buffer, ".exit") == 0) {
        closeInputBuffer(inputBuffer);
        exit(EXIT_SUCCESS);
    }
     else {
         return META_COMMAND_UNRECOGNIZED_COMMAND;
     }
}



int main(int argc, char *argv[]) {
    InputBuffer* inputBuffer = newInputBuffer();
    while(1) {
        printPrompt();
        readInput(inputBuffer);

        if (inputBuffer->buffer[0] == '.') {
            switch(doMetaCommand(inputBuffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command %s\n", inputBuffer->buffer);
                    continue;
            }
                
        }

        Statement statement;
        switch(prepareStatement(inputBuffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of %s\n", inputBuffer->buffer);
                continue;
        }
        executeStatement(&statement);
        printf("Executed.\n");
    }
}
