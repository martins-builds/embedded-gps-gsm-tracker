#include "gsm_task.h"
#include <stdint.h>

static uint16_t gsm_rx_index = 0;
volatile uint8_t gsm_response_ready;
SemaphoreHandle_t gsm_response_sem;

void uart3_init(void){
    // alternate function modes
    GPIOC->MODER |= ((1 << 9) | (1 << 11)); 
    GPIOC->MODER &= ~((1 << 8) | (1 << 10));

    GPIOC->AFRL &= ~((0xF << 16) | (0xF << 20));   // clear both 4-bit fields first
    GPIOC->AFRL |=  ((7 << 16) | (7 << 20));        // AF7 for PA9, PA10

    RCC->APB1ENR1 |= (1 << 18);

    USART3->BRR = 8333;
    USART3->CR1 |= (1 << 0) | (1 << 2) | (1 << 3);

    USART3->CR1 |= (1 << 5);   // RXNEIE - enable RX-not-empty interrupt
    NVIC->ISER[1] |= (1 << 7);   // IRQ39 = USART3
}
void gsm_task_init(void){
    gsm_response_sem = xSemaphoreCreateBinary();
}
uint8_t buffer_ends_with(char *buf, uint16_t index, const char *suffix){
    uint16_t suffix_len = strlen(suffix);
    if (index < suffix_len) return 0;   // not enough bytes yet to possibly match

    for (uint16_t i = 0; i < suffix_len; i++) {
        if (buf[index - suffix_len + i] != suffix[i]) {
            return 0;
        }
    }
    return 1;
}
void USART3_IRQHandler(void){
    uint8_t byte = USART3->RDR;

    if (gsm_rx_index < GSM_BUF_SIZE - 1) {
        gsm_rx_buffer[gsm_rx_index] = byte;
        gsm_rx_index++;
        gsm_rx_buffer[gsm_rx_index] = '\0';

        if (buffer_ends_with(gsm_rx_buffer, gsm_rx_index, "OK\r\n") ||
            buffer_ends_with(gsm_rx_buffer, gsm_rx_index, "ERROR\r\n")) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(gsm_response_sem, &xHigherPriorityTaskWoken);
            gsm_rx_index = 0;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
void uart3_send_byte(uint8_t byte){
    while (!(USART3->ISR & (1 << 7)));   // wait for TXE (transmit register empty)
    USART3->TDR = byte;
}

void uart3_send_string(const char *str){
    while (*str) {
        uart3_send_byte((uint8_t)*str);
        str++;
    }
}
void gsm_send_command(const char *cmd){
    gsm_response_ready = 0;
    uart3_send_string(cmd);
    uart3_send_string("\r\n");
}
uint8_t gsm_send_and_wait(const char *cmd, uint8_t max_retries){
    for (uint8_t attempt = 0; attempt < max_retries; attempt++) {
        gsm_send_command(cmd);

        if (xSemaphoreTake(gsm_response_sem, pdMS_TO_TICKS(5000)) == pdTRUE) {
            if (strstr(gsm_rx_buffer, "OK") != NULL) {
                return 1;   // success
            }
            // response arrived but wasn't OK (likely ERROR) - fall through to retry
        }
        // else: timed out - fall through to retry

        vTaskDelay(pdMS_TO_TICKS(500));   // brief pause before retrying
    }
    return 0;   // all retries exhausted, genuine failure
}
void gsm_task(void *pvParameters){
    if (!gsm_send_and_wait("AT", 3)) {
        // handle total failure - e.g. display error on OLED, or abort
    }

    if (!gsm_send_and_wait("AT+CPIN?", 3)) {
        // handle failure
    }

    if (!gsm_send_and_wait("AT+CREG?", 3)) {
        // handle failure
    }

    // ... continue through CGATT=1, SAPBR, HTTPINIT, HTTPPARA, HTTPACTION, HTTPREAD
}