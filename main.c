#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char* buffer;
    size_t bufferLength;
    ssize_t inputLength;
} InputBuffer;

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

void closeInputBuffer(InputBuffer* inputBuffer) {
    free(inputBuffer->buffer);
    free(inputBuffer);
}

int main(int argc, char *argv[]) {
    InputBuffer* inputBuffer = newInputBuffer();
    while(1) {
        printPrompt();
        readInput(inputBuffer);

        if (strcmp(inputBuffer->buffer, ".exit") == 0) {
            closeInputBuffer(inputBuffer);
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Unrecognized command %s\n", inputBuffer->buffer);
        }
    }
}
