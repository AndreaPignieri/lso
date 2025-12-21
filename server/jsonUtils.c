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
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Nervosa. Usa un vocabolario calmante e rassicurante nelle tue risposte, chiedigli come sta e cerca di farlo sentire a suo agio.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Nervosa. Cercherò di rendere questa conversazione il più piacevole possibile per te!");
            break;
        case OPEN:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Aperta. Usa un vocabolario entusiasta e coinvolgente nelle tue risposte, chiedigli dei suoi interessi e cerca di stimolare una conversazione aperta.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Aperta. Non vedo l'ora di conoscere i tuoi interessi!");
            break;
        case RELAXED:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Rilassata. Usa un vocabolario amichevole e facile da gestire nelle tue risposte, mantieni la conversazione leggera e piacevole.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Rilassata. Cercherò di rendere questa conversazione il più piacevole possibile per te!");
            break;
        case SERIOUS:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Seria. Usa un vocabolario formale e rispettoso nelle tue risposte, concentra l'attenzione su argomenti significativi e evita chiacchere.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Seria. Cercherò di rendere questa conversazione il più piacevole possibile per te!");
            break;
        case TIMID:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Timida. Usa un vocabolario gentile e incoraggiante nelle tue risposte, fai domande aperte e cerca di costruire la sua fiducia.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Timida. Cercherò di rendere questa conversazione il più piacevole possibile per te!");
            break;
        case NEUTRAL:
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "config", "Comportati come un robot che interagisce con un essere umano. La personalità dell'umano è stata calcolata come Neutra. Usa un vocabolario bilanciato e adattabile nelle tue risposte, adatta il tuo tono in base al flusso della conversazione.");
            cJSON_AddStringToObject(response, "toSpeak", "La tua personalità è stata classificata come Neutra. Cercherò di rendere questa conversazione il più piacevole possibile per te!");
            break;
        default:
            createErrorResponse("Unknown dialogue type");
            break;
    }
    return response;
}