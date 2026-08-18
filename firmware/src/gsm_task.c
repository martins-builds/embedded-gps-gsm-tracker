#include "gsm_task.h"
#include <stdint.h>

void uart3_init(void){
    // alternate function modes
    GPIOA->MODER |= ((1 << 9) | (1 << 11)); 
    GPIOA->MODER &= ~((1 << 8) | (1 << 10));

    GPIOA->AFRH &= ~((0xF << 16) | (0xF << 20));   // clear both 4-bit fields first
    GPIOA->AFRH |=  ((7 << 16) | (7 << 20));        // AF7 for PA9, PA10

    RCC->APB2ENR |= (1 << 14);

    USART1->BRR = 8333;
    USART1->CR1 |= (1 << 0) | (1 << 2) | (1 << 3);

    USART1->CR1 |= (1 << 5);      // RXNEIE - enable RX-not-empty interrupt
    NVIC->ISER[1] |= (1 << 5);   // IRQ37 = USART1
}