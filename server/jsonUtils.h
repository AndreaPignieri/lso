#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "cJSON.h"

cJSON* processRequest(cJSON *request_json);
cJSON* createErrorResponse(const char* message);
cJSON* handleGreeting(int language);
cJSON* italianTIPI();
cJSON* englishTIPI();

#endif