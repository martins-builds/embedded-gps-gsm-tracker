#ifndef GPS_TASK_H
#define GPS_TASK_H

#include "stm32l476re.h"

typedef struct{
    float latitude;
    float longitude;
    uint8_t hours, minutes, seconds;
    uint8_t valid;
} GPS_Data_t;

void gps_parse_gprmc(const char *sentence, GPS_Data_t *result);

#endif