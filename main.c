#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "enums.h"
#include "structs.h"
#include "row.h"
#include "table.h"
#include "constants.h"


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
        int argsAssigned = sscanf(inputBuffer->buffer, "insert %d %s %s", &(statement->rowToInsert.id), statement->rowToInsert.username,statement->rowToInsert.email);
        if (argsAssigned != 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if (strcmp(inputBuffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void* rowSlot(Table* table, int rowNum) {
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    void* page = table->pages[pageNum];
    if (page == NULL) {
        page = table->pages[pageNum] = malloc(PAGE_SIZE);
    }
    uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
    uint32_t byteOffset = rowOffset * ROW_SIZE;
    return page + byteOffset;
}

void serializeRow(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserializeRow(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE); 
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

ExecuteResult executeInsert(Statement* statement, Table* table) {
    if (table->numRows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row* rowToInsert = &(statement->rowToInsert);
    serializeRow(rowToInsert, rowSlot(table, table->numRows));
    table->numRows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult executeSelect(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->numRows; i++) {
        deserializeRow(rowSlot(table, i), &row);
        printRow(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult executeStatement(Statement* statement, Table* table) {
    switch(statement->type){
        case (STATEMENT_INSERT):
            return executeInsert(statement, table);
        case (STATEMENT_SELECT):
            return executeSelect(statement, table);
    }
}

void closeInputBuffer(InputBuffer* inputBuffer) {
    free(inputBuffer->buffer);
    free(inputBuffer);
}

MetaCommandResult doMetaCommand(InputBuffer* inputBuffer, Table* table) {
    if (strcmp(inputBuffer->buffer, ".exit") == 0) {
        closeInputBuffer(inputBuffer);
        freeTable(table);
        exit(EXIT_SUCCESS);
    }
     else {
         return META_COMMAND_UNRECOGNIZED_COMMAND;
     }
}



int main(int argc, char *argv[]) {
    InputBuffer* inputBuffer = newInputBuffer();
    Table* table = newTable();
    while(1) {
        printPrompt();
        readInput(inputBuffer);

        if (inputBuffer->buffer[0] == '.') {
            switch(doMetaCommand(inputBuffer, table)) {
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
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of %s\n", inputBuffer->buffer);
                continue;
        }
        switch(executeStatement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Execute success.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error. Table full.\n");
                break;
        }
        printf("Executed.\n");
    }
}
