#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "jsonUtils.h"
#include "tipi.h" 
#include "errors.h"
#include "strings.h"


cJSON* createErrorResponse(const char* message) 
{
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "error");
    cJSON_AddStringToObject(response, "message", message);
    return response;
}

cJSON* processRequest(cJSON *request_json) 
{
    cJSON *typeItem = cJSON_GetObjectItemCaseSensitive(request_json, "type");
    
    if (!cJSON_IsString(typeItem)) {
        return createErrorResponse("Missing or invalid request type");
    }
    if (strcmp(typeItem->valuestring, "get_questions") == 0) {
        return italianTIPI(); 
    }
    else if (strcmp(typeItem->valuestring, "tipi_submission") == 0) {
        return handleTipiSubmission(request_json); 
    }
    return createErrorResponse("Unknown request type");
}

cJSON* italianTIPI()
{
    cJSON *response = cJSON_CreateObject();
    
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message", MSG_TIPI_INTRO);
    
    cJSON *questions = cJSON_AddArrayToObject(response, "questions");
    
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_1));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_2));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_3));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_4));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_5));
    
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_6)); 
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_7));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_8));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_9));
    cJSON_AddItemToArray(questions, cJSON_CreateString(QUEST_10));
    
    return response;
}

cJSON* handleTipiSubmission(cJSON *request_json) {
    cJSON *responsesItem = cJSON_GetObjectItemCaseSensitive(request_json, "scores");

    if (!cJSON_IsArray(responsesItem) || cJSON_GetArraySize(responsesItem) != 10) {
        return createErrorResponse("Invalid or missing responses array");
    }

    personality p = calculateTIPIPersonality(responsesItem);

    if (!isValid(p)) {
        return createErrorResponse("Invalid response values");
    }
    
    dialogueType dType = determineDialogueType(p);
    return startDialogue(dType);
}

cJSON* startDialogue(dialogueType dType) 
{
    cJSON *response = cJSON_CreateObject();
    switch (dType) 
    {
        case NERVOUS:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_NERVOUS);
            cJSON_AddStringToObject(response, "toSpeak", SPK_NERVOUS);
            break;
        case OPEN:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_OPEN);
            cJSON_AddStringToObject(response, "toSpeak", SPK_OPEN);
            break;
        case RELAXED:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_RELAXED);
            cJSON_AddStringToObject(response, "toSpeak", SPK_RELAXED);
            break;
        case SERIOUS:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_SERIOUS);
            cJSON_AddStringToObject(response, "toSpeak", SPK_SERIOUS);
            break;
        case TIMID:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_TIMID);
            cJSON_AddStringToObject(response, "toSpeak", SPK_TIMID);
            break;
        case NEUTRAL:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", CFG_NEUTRAL);
            cJSON_AddStringToObject(response, "toSpeak", SPK_NEUTRAL);
            break;
        default:
            createErrorResponse("Unknown dialogue type");
            break;
    }
    return response;
}