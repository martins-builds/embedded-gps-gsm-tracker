#include "gsm_task.h"
#include <stdint.h>

static uint16_t gsm_rx_index = 0;

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
        gsm_rx_buffer[gsm_rx_index] = '\0';   // keep it null-terminated as we go

        if (buffer_ends_with(gsm_rx_buffer, gsm_rx_index, "OK\r\n") ||
            buffer_ends_with(gsm_rx_buffer, gsm_rx_index, "ERROR\r\n")) {
            gsm_response_ready = 1;
            gsm_rx_index = 0;
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
    
}