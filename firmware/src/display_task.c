#include "display_task.h"
#include <stdint.h>

void i2c_init(void){
    GPIOB->MODER |= (1 << 13); // alternate function modes
    GPIOB->MODER &= ~(1 << 12);
    GPIOB->MODER |= (1 << 15);
    GPIOB->MODER &= ~(1 << 14);

    GPIOB->OTYPER |= (1 << 6); //open drain output
    GPIOB->OTYPER |= (1 << 7);

    GPIOB->AFRL &= ~(1 << 24);   // clear PB6's AF field (bits 27:24)
    GPIOB->AFRL |=  (4 << 24);     // AF4 for PB6

    GPIOB->AFRL &= ~(1 << 28);   // clear PB7's AF field (bits 31:28)
    GPIOB->AFRL |=  (4 << 28);     // AF4 for PB7
    RCC->APB1ENR1 |= (1 << 21); // enable 12c1 for display

    // TIMINGR and SSD1306 init sequence. Reading the TIMINGR register and i dont know which to set
    
    I2C->CR1 |= (1 << 0); //periferal enable
}