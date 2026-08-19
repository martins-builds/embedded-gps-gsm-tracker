#ifndef GSM_TASK_H
#define GSM_TASK_H

#include "stm32l476re.h"
#include "core_cm4.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define GSM_BUF_SIZE 200
extern char gsm_rx_buffer[GSM_BUF_SIZE];
extern volatile uint8_t gsm_response_ready;
extern SemaphoreHandle_t gsm_response_sem;

void uart3_init(void);
void gsm_task_init(void);
uint8_t buffer_ends_with(char *buf, uint16_t index, const char *suffix);
void USART3_IRQHandler(void);
void uart3_send_byte(uint8_t byte);
void uart3_send_string(const char *str);
void gsm_send_command(const char *cmd);

#endif