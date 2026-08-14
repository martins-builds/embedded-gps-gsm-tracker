#include "display_task.h"
#include <stdint.h>

void i2c_init(void){
    GPIOB->MODER |= (1 << 13); // alternate function modes
    GPIOB->MODER &= ~(1 << 12);
    GPIOB->MODER |= (1 << 15);
    GPIOB->MODER &= ~(1 << 14);
    GPIOB->OTYPER |= (1 << 6); //open drain output
    GPIOB->OTYPER |= (1 << 7);
    RCC->APB1ENR1 |= (1 << 21); // enable 12c1 for display

    I2C->CR1 |= (1 << 0); //periferal enable

    // TIMINGR and SSD1306 init sequence
}