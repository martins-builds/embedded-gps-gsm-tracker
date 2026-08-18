#ifndef GSM_TASK_H
#define GSM_TASK_H

#include "stm32l476re.h"
#include "core_cm4.h"

#define GSM_BUF_SIZE 200
extern char gsm_rx_buffer[GSM_BUF_SIZE];
extern volatile uint8_t gsm_response_ready;

void uart3_init(void);
void USART3_IRQHandler(void);

#endif