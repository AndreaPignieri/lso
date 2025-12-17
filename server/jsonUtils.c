#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "jsonUtils.h"
#include "tipi.h" 
#include "errors.h"


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

//TODO refactor questions storage

cJSON* italianTIPI()
{
    cJSON *response = cJSON_CreateObject();
    
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message", "Adesso ti farò delle domande per valutare la tua personalità : rispondi alle 10 domande con un punteggio da 1 a 7, dove 1 significa 'fortemente in disaccordo' e 7 significa 'fortemente d'accordo'.");
    
    cJSON *questions = cJSON_AddArrayToObject(response, "questions");
    
    cJSON_AddItemToArray(questions, cJSON_CreateString("1. Sono una persona estroversa, esuberante."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("2. Sono una persona polemica, litigiosa."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("3. Sono una persona affidabile, auto-disciplinata."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("4. Sono una persona ansiosa, che si agita facilmente."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("5. Sono una persona aperta alle nuove esperienze, con molti interessi."));
    
    cJSON_AddItemToArray(questions, cJSON_CreateString("6. Sono una persona riservata, silenziosa.")); 
    cJSON_AddItemToArray(questions, cJSON_CreateString("7. Sono una persona comprensiva, affettuosa."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("8. Sono una persona disorganizzata, distratta."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("9. Sono una persona tranquilla, emotivamente stabile."));
    cJSON_AddItemToArray(questions, cJSON_CreateString("10. Sono una persona tradizionalista, abitudinaria."));
    
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


//TODO improve prompts

cJSON* startDialogue(dialogueType dType) 
{
    cJSON *response = cJSON_CreateObject();
    switch (dType) 
    {
        case NERVOUS:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Nervous. Use a calming and soothing vocabulary in your responses, ask about how he is doing and try to make him feel at ease.");
            break;
        case OPEN:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Open. Use an enthusiastic and engaging vocabulary in your responses, ask about his interests and try to stimulate an open conversation.");
            break;
        case RELAXED:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Relaxed. Use a friendly and easy-going vocabulary in your responses, keep the conversation light and enjoyable.");
            break;
        case SERIOUS:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Serious. Use a formal and respectful vocabulary in your responses, focus on meaningful topics and avoid small talk.");
            break;
        case TIMID:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Timid. Use a gentle and encouraging vocabulary in your responses, ask open-ended questions and try to build his confidence.");
            break;
        case NEUTRAL:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Act as a Robot interacting with a human. The human's personality has been calculated as Neutral. Use a balanced and adaptable vocabulary in your responses, adjust your tone based on the flow of the conversation.");
            break;
        default:
            createErrorResponse("Unknown dialogue type");
            break;
    }
    return response;
}