#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "cJSON.h"
#include "tipi.h"

cJSON* processRequest(cJSON *request_json);
cJSON* createErrorResponse(const char* message);
cJSON* handleLanguage(char *language);
cJSON* italianTIPI();
cJSON* englishTIPI();
cJSON* handleTipiSubmission(cJSON *request_json);
cJSON* startDialogue(dialogueType dType);

#endif