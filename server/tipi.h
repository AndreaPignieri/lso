#ifndef TIPI_H
#define TIPI_H

#include "cJSON.h"

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
    RELAXED,
    SERIOUS,
    TIMID,
    NEUTRAL
} dialogueType;

int isValid(personality p);
personality calculateTIPIPersonality(cJSON *responsesItem);
dialogueType determineDialogueType(personality p);

#endif