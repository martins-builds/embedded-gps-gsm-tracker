#include "gps_task.h"
#include <string.h>
#include <stdlib.h>

char gps_rx_buffer[GPS_BUF_SIZE];
volatile uint8_t gps_line_ready = 0;
static uint16_t gps_rx_index = 0;
GPS_Data_t gps_data;

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

    result->valid = (fields[2][0] == 'A') ? 1 : 0;

    // fields[1] = time "HHMMSS"
    char time_str[3];
    time_str[2] = '\0';

    time_str[0] = fields[1][0]; time_str[1] = fields[1][1];
    result->hours = atoi(time_str);

    time_str[0] = fields[1][2]; time_str[1] = fields[1][3];
    result->minutes = atoi(time_str);

    time_str[0] = fields[1][4]; time_str[1] = fields[1][5];
    result->seconds = atoi(time_str);

    // fields[3] = latitude "DDMM.MMMM", fields[4] = hemisphere N/S
    char lat_deg_str[3] = { fields[3][0], fields[3][1], '\0' };
    float lat_deg = atof(lat_deg_str);
    float lat_min = atof(&fields[3][2]);   // rest of string from index 2 onward
    result->latitude = lat_deg + (lat_min / 60.0f);
    if (fields[4][0] == 'S') result->latitude = -result->latitude;

    // fields[5] = longitude "DDDMM.MMMM", fields[6] = hemisphere E/W
    char lon_deg_str[4] = { fields[5][0], fields[5][1], fields[5][2], '\0' };
    float lon_deg = atof(lon_deg_str);
    float lon_min = atof(&fields[5][3]);   // rest of string from index 3 onward
    result->longitude = lon_deg + (lon_min / 60.0f);
    if(fields[6][0] == 'W') result->longitude = -result->longitude;
}
void uart1_init(void){
    // alternate function modes
    GPIOA->MODER |= ((1 << 19) | (1 << 21)); 
    GPIOA->MODER &= ~((1 << 18) | (1 << 20));

    GPIOA->AFRH &= ~((0xF << 4) | (0xF << 8));   // clear both 4-bit fields first
    GPIOA->AFRH |=  ((7 << 4) | (7 << 8));        // AF7 for PA9, PA10

    RCC->APB2ENR |= (1 << 14);

    USART1->BRR = 8333;
    USART1->CR1 |= (1 << 0) | (1 << 2) | (1 << 3);

    USART1->CR1 |= (1 << 5);      // RXNEIE - enable RX-not-empty interrupt
    NVIC->ISER[1] |= (1 << 5);   // IRQ37 = USART1
}
void USART1_IRQHandler(void){
    uint8_t byte = USART1->RDR;   // reading RDR also clears RXNE automatically

    if (byte == '\n') {
        gps_rx_buffer[gps_rx_index] = '\0';   // null-terminate the completed line
        gps_line_ready = 1;
        gps_rx_index = 0;                      // reset for the next sentence
    }
    else{
        if (gps_rx_index < GPS_BUF_SIZE - 1) {
            gps_rx_buffer[gps_rx_index] = byte;
            gps_rx_index++;
        }
        // else: buffer full, byte silently dropped (overflow guard)
    }
}
void gps_process(void){
    if (gps_line_ready) {
        gps_parse_gprmc(gps_rx_buffer, &gps_data);
        gps_line_ready = 0;
    }
}
void gps_task(void *pvParameters){
    while (1) {
        gps_process();
        vTaskDelay(pdMS_TO_TICKS(100));  // yield to other tasks periodically
    }
}