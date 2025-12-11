#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "errors.h"
#include "cJSON.h"
#include "jsonUtils.h"
#include "clientHandler.h"

#define TRUE 1

void *clientHandler(void* socketPtr) {
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
        return NULL;
    }

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        errorHandler(clientSocket, JSON_ERROR);
        return NULL;
    }
    
    cJSON *response = processRequest(json);
    if (response == NULL) {
        cJSON_Delete(json);
        errorHandler(clientSocket, REQUEST_PROCESSING_ERROR);
        return NULL;
    }
    
    //TODO: FIX ERROR HANDLING
    cJSON *statusItem = cJSON_GetObjectItemCaseSensitive(response, "status");
    if (cJSON_IsString(statusItem) && strcmp(statusItem->valuestring, "error") == 0) {
        cJSON_Delete(json);
        errorHandler(clientSocket, RESPONSE_CREATION_ERROR);
        cJSON_Delete(response);
        return NULL;
    }

    char *responseStr = cJSON_PrintUnformatted(response);
    if (responseStr == NULL) {
        cJSON_Delete(response);
        cJSON_Delete(json);
        errorHandler(clientSocket, RESPONSE_CREATION_ERROR);
        return NULL;
    }
    send(clientSocket, responseStr, strlen(responseStr), 0);


    free(responseStr);
    cJSON_Delete(response);
    cJSON_Delete(json);
    close(clientSocket);

    return NULL;
}