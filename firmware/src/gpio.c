#include "gpio.h"

void gpio_init(){
    RCC->AHB2ENR |= (1 << 2); //gpioc enabled

    //setting PC2 and PC3 as inputs
    GPIOC->MODER &= ~(1 << 2);
    GPIOC->MODER &= ~(1 << 3);

    GPIOC->MODER &= ~(1 << 4);
    GPIOC->MODER &= ~(1 << 5);

}