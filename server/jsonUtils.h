#include "cJSON.h"

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

typedef struct {
    double extraversion;
    double agreeableness;
    double conscientiousness;
    double neuroticism;
    double openness;
} personality;

personality calculatePersonality(const char* jsonStr);
//cJSON*  

#endif