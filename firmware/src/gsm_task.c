#include "gsm_task.h"
#include <stdint.h>

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

void USART3_IRQHandler(void){
    
}