#ifndef GPS_TASK_H
#define GPS_TASK_H

#include "stm32l476re.h"
#include "core_cm4.h"
#include "FreeRTOS.h"

typedef struct{
    float latitude;
    float longitude;
    uint8_t hours, minutes, seconds;
    uint8_t valid;
} GPS_Data_t;

#define GPS_BUF_SIZE 100
extern char gps_rx_buffer[GPS_BUF_SIZE];
extern volatile uint8_t gps_line_ready;
extern GPS_Data_t gps_data;
extern MutexHandle_t gps_data_mutex;

void gps_parse_gprmc(const char *sentence, GPS_Data_t *result);
void uart1_init(void);
void USART1_IRQHandler(void);
void gps_process(void);
void gps_task(void *pvParameters);

#endif