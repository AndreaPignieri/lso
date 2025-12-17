#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "cJSON.h"
#include "errors.h"

void errorHandler(int clientSocket, const char* errorMessage) {
    printf("[SERVER ERROR LOG]: %s\n", errorMessage);

    cJSON *error = cJSON_CreateObject();
    if (error == NULL) {
        return;
    }

    cJSON_AddStringToObject(error, "status", "error");
    cJSON_AddStringToObject(error, "message", errorMessage);

    char *json_str = cJSON_PrintUnformatted(error); 

    if (json_str != NULL) {
        send(clientSocket, json_str, strlen(json_str), 0);
        free(json_str);
    }

    cJSON_Delete(error);
}