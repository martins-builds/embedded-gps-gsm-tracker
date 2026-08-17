#ifndef GPS_TASK_H
#define GPS_TASK_H

#include "stm32l476re.h"

typedef struct{
    float latitude;
    float longitude;
    uint8_t hours, minutes, seconds;
    uint8_t valid;
} GPS_Data_t;

#define GPS_BUF_SIZE 100
extern char gps_rx_buffer[GPS_BUF_SIZE];
extern volatile uint8_t gps_line_ready;

void gps_parse_gprmc(const char *sentence, GPS_Data_t *result);
void uart1_init(void);

#endif