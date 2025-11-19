#include <stdlib.h>
#include <unistd.h>

#include "functions.h"

void clientHandler(void* socketPtr) {
    int clientSocket = *((int*)socketPtr);
    free(socketPtr);

    

    

    close(clientSocket);
}