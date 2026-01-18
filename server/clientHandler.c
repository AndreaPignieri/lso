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

void performCleanup(int socket, cJSON *req, cJSON *resp, char *respStr) {
    if (respStr != NULL) free(respStr);
    if (resp != NULL) cJSON_Delete(resp);
    if (req != NULL) cJSON_Delete(req);
    close(socket);
}

void *clientHandler(void* socketPtr) {
    int clientSocket = *((int*)socketPtr);
    free(socketPtr);

    char buf[1024];
    cJSON *json = NULL;
    cJSON *response = NULL;
    char *responseStr = NULL;

    ssize_t n = recv(clientSocket, buf, sizeof(buf)-1, 0);

    if (n > 0)
    {
        buf[n] = '\0';
    }
    else {
        errorHandler(clientSocket, DATA_RECEPTION_ERROR);
        performCleanup(clientSocket, NULL, NULL, NULL);
        return NULL;
    }

    json = cJSON_Parse(buf);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        errorHandler(clientSocket, JSON_ERROR);
        performCleanup(clientSocket, json, NULL, NULL);
        return NULL;
    }
    
    response = processRequest(json);
    if (response == NULL) {
        errorHandler(clientSocket, REQUEST_PROCESSING_ERROR);
        performCleanup(clientSocket, json, NULL, NULL);
        return NULL;
    }
    
    cJSON *statusItem = cJSON_GetObjectItemCaseSensitive(response, "status");
    if (cJSON_IsString(statusItem) && strcmp(statusItem->valuestring, "error") == 0) {
        cJSON *errorMessageItem = cJSON_GetObjectItemCaseSensitive(response, "message");
        const char *errorMessage = cJSON_IsString(errorMessageItem) ? errorMessageItem->valuestring : "Unknown error";
        errorHandler(clientSocket, errorMessage);
        performCleanup(clientSocket, json, response, NULL);
        return NULL;
    }

    responseStr = cJSON_PrintUnformatted(response);
    if (responseStr == NULL) {
        errorHandler(clientSocket, RESPONSE_CREATION_ERROR);
        performCleanup(clientSocket, json, response, NULL);
        return NULL;
    }
    send(clientSocket, responseStr, strlen(responseStr), 0);

    performCleanup(clientSocket, json, response, responseStr);

    return NULL;
}