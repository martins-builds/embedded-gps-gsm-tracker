#include "display_task.h"
#include <stdint.h>

void i2c_init(void){
    // alternate function modes
    GPIOB->MODER |= ((1 << 13) | (1 << 15)); 
    GPIOB->MODER &= ~((1 << 12) | (1 << 14));

    GPIOB->OTYPER |= (1 << 6); //open drain output
    GPIOB->OTYPER |= (1 << 7);

    GPIOB->AFRL &= ~((1 << 24) | (1 << 28)); //clear AF field
    GPIOB->AFRL |=  ((4 << 24) | (4 << 28)); //set AF fields     // AF4 for PB6

    RCC->APB1ENR1 |= (1 << 21); // enable 12c1 for display

    // TIMINGR and SSD1306 init sequence. Reading the TIMINGR register and i dont know which to set
    I2C1->TIMINGR = 0x00300F38;
    I2C1->CR1 |= (1 << 0); //periferal enable
}