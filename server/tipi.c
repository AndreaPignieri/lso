#include "tipi.h"

int isValid(personality p) 
{
    if (p.extraversion < 0 || p.agreeableness < 0 || p.conscientiousness < 0 || p.neuroticism < 0 || p.openness < 0) 
    {
        return 0;
    }
    return 1;
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
    if (p.neuroticism >= 5.0) 
    {
        return NERVOUS;
    }

    if (p.extraversion <= 3.5) 
    {
        return TIMID;
    }

    if (p.openness >= 5.5 || (p.extraversion >= 5.5 && p.openness >= 4.5))
    {
        return OPEN;
    }

    if (p.agreeableness >= 5.0)
    {
        return RELAXED;
    }

    if (p.conscientiousness >= 5.5)
    {
        return SERIOUS;
    }

    return NEUTRAL;
}
