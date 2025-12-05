#include "cJSON.h"

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

typedef struct
{
    double extraversion;
    double agreeableness;
    double conscientiousness;
    double neuroticism;
    double openness;
} personality;

typedef enum
{
    NERVOUS,
    OPEN,
    RELAXED
} dialogueType;

cJSON* processRequest(cJSON *request_json);
cJSON* createErrorResponse(const char* message);
cJSON* handleGreeting(int language);
cJSON* handleTipiSubmission(cJSON *request_json);
personality calculateTIPIPersonality(cJSON *responsesItem);

#endif