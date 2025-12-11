#include <stdio.h>
#include <unistd.h>
#include "errors.h"

void errorHandler(int clientSocket, const char* errorMessage) {
    printf("Error: %s\n", errorMessage);
    send(clientSocket, errorMessage, strlen(errorMessage), 0);
}