#ifndef GPS_TASK_H
#define GPS_TASK_H

#include "stm32l476re.h"
#include "core_cm4.h"

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
void USART1_IRQHandler(void);

#endif