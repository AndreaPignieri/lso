#include <stdlib.h>
#include <unistd.h>

#include "cJSON.h"
#include "jsonUtils.h"
#include "errors.h"

#define ITALIAN 1
#define ENGLISH 2


cJSON* processRequest(cJSON *request_json) {
    cJSON *typeItem = cJSON_GetObjectItemCaseSensitive(request_json, "type");
    
    if (!cJSON_IsString(typeItem)) {
        return createErrorResponse("Missing or invalid request type");
    }

    if (strcmp(typeItem->valuestring, "greeting") == 0) {
        int language = cJSON_GetObjectItemCaseSensitive(request_json, "language");
        return handleGreeting(language);
    }
    else if (strcmp(typeItem->valuestring, "tipi_submission") == 0) {
        return handleTipiSubmission(request_json); 
    } 
    return createErrorResponse("Unknown request type");
}

cJSON* createErrorResponse(const char* message) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "error");
    cJSON_AddStringToObject(response, "message", message);
    return response;
}

cJSON* handleGreeting(int language) {
    if(language == ITALIAN)
        return italianTIPI();
    else if (language == ENGLISH)
        return englishTIPI();
    else
        return createErrorResponse("Unsupported language");
}

cJSON* italianTIPI()
{
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message", "Ecco le tue istruzioni per il test TIPI: Rispondi alle 10 domande con un punteggio da 1 a 7, dove 1 significa 'fortemente in disaccordo' e 7 significa 'fortemente d'accordo'.");
    cJSON_AddArrayToObject(response, "questions");
    cJSON_AddItemToArray(response->child, cJSON_CreateString("1. Sono una persona estroversa, esuberante."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("2. Sono una persona polemica, litigiosa."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("3. Sono una persona affidabile, auto-disciplinata."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("4. Sono una persona ansiosa, che si agita facilmente."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("5. Sono una persona aperta alle nuove esperienze, con molti interessi."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("6. Sono una persona aperta riservata, silenziosa."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("7. Sono una persona aperta comprensiva, affettuosa."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("8. Sono una persona aperta disorganzzata, distratta."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("9. Sono una persona aperta tranquilla, emotivamente stabile."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("10. Sono una persona aperta tradizionalista, abitudinaria."));
    return response;
}

cJSON* englishTIPI()
{
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message", "Here are your instructions for the TIPI test: Answer the 10 questions with a score from 1 to 7, where 1 means 'strongly disagree' and 7 means 'strongly agree'.");
    cJSON_AddArrayToObject(response, "questions");
    cJSON_AddItemToArray(response->child, cJSON_CreateString("1. I see myself as extraverted, enthusiastic."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("2. I see myself as critical, quarrelsome."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("3. I see myself as dependable, self-disciplined."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("4. I see myself as anxious, easily upset."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("5. I see myself as open to new experiences, complex."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("6. I see myself as reserved, quiet."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("7. I see myself as sympathetic, warm."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("8. I see myself as disorganized, careless."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("9. I see myself as calm, emotionally stable."));
    cJSON_AddItemToArray(response->child, cJSON_CreateString("10. I see myself as conventional, uncreative."));
    return response;
}

cJSON* handleTipiSubmission(cJSON *request_json) {
    cJSON *responsesItem = cJSON_GetObjectItemCaseSensitive(request_json, "responses");

    if (!cJSON_IsArray(responsesItem) || cJSON_GetArraySize(responsesItem) != 10) {
        return createErrorResponse("Invalid or missing responses array");
    }

    personality p = calculateTIPIPersonality(responsesItem);

    if (p.extraversion == -1) {
        return createErrorResponse("Invalid response values");
    }
    
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddNumberToObject(response, "extraversion", p.extraversion);
    cJSON_AddNumberToObject(response, "agreeableness", p.agreeableness);
    cJSON_AddNumberToObject(response, "conscientiousness", p.conscientiousness);
    cJSON_AddNumberToObject(response, "neuroticism", p.neuroticism);
    cJSON_AddNumberToObject(response, "openness", p.openness);

    return response;
}

personality calculateTIPIPersonality(cJSON *responsesItem) 
{
    int answers[10];
    for (int i = 0; i < 10; i++) {
        cJSON *answerItem = cJSON_GetArrayItem(responsesItem, i);
        if (!cJSON_IsNumber(answerItem)) {
            personality invalid = {-1, -1, -1, -1, -1};
            return invalid; 
        }
        answers[i] = answerItem->valueint;
    }

    personality p;
    p.extraversion = (answers[0] + (8 - answers[5]));
    p.agreeableness = ((8 - answers[1]) + answers[6]);
    p.conscientiousness = (answers[2] + (8 - answers[7]));
    p.neuroticism = (answers[3] + (8 - answers[8]));
    p.openness = (answers[4] + (8 - answers[9]));

    return p;
}

dialogueType determineDialogueType(personality p) 
{
    if (isHigh(p.neuroticism) && !isHigh(p.openness) && !isHigh(p.extraversion)) 
    {
        return NERVOUS;
    }
    if (isHigh(p.openness) && isHigh(p.extraversion))
    {
        return OPEN;
    }
    if (isHigh(p.agreeableness) && isHigh(p.conscientiousness) && !isHigh(p.neuroticism))
    {
        return RELAXED;
    }
}

int isHigh(double score) 
{
    return score >= 3.5;
}

