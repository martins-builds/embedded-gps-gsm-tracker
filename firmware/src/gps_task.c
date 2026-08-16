#include "gps_task.h"
#include <string.h>

void gps_parse_gprmc(const char *sentence, GPS_Data_t *result){
    char sentence_copy[100];
    strcpy(sentence_copy, sentence);   // work on a copy, since strtok() destroys the original

    char *fields[12];
    char *token = strtok(sentence_copy, ",");
    int i = 0;
    while (token != NULL && i < 12){
        fields[i] = token;
        i++;
        token = strtok(NULL, ",");
    }
    /*for (int i = 0; i < 12; i++)
    {
        
    }*/
}
