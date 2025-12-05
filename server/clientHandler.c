#include <stdlib.h>
#include <unistd.h>

#include "errors.h"
#include "cJSON.h"
#include "clientHandler.h"

void clientHandler(void* socketPtr) {
    int clientSocket = *((int*)socketPtr);
    free(socketPtr);

    char buf[1024];
    ssize_t n = recv(clientSocket, buf, sizeof(buf)-1, 0);

    if (n > 0)
    {
        buf[n] = '\0';
    }
    else {
        errorHandler(clientSocket, DATA_RECEPTION_ERROR);
        return;
    }

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        errorHandler(clientSocket, JSON_ERROR);
        return;
    }
    
    cJSON *response = processRequest(json);
    char *responseStr = cJSON_PrintUnformatted(response);
    send(clientSocket, responseStr, strlen(responseStr), 0);
    
    

    free(responseStr);
    cJSON_Delete(response);
    cJSON_Delete(json);
    close(clientSocket);

    return NULL;
}