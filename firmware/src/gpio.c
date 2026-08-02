#include "gpio.h"

void gpio_init(){
    RCC->AHB2ENR |= (1 << 2); //gpioc enabled

    //setting PC2 and PC3 as inputs
    GPIOC->MODER &= ~(1 << 4); //PC2
    GPIOC->MODER &= ~(1 << 5);
    GPIOC->MODER &= ~(1 << 6); //PC3
    GPIOC->MODER &= ~(1 << 7);

    //setting PC2 and PC3 as pullup
    GPIOC->PUPDR &= ~(1 << 4); //PC2 clear
    GPIOC->PUPDR &= ~(1 << 5);
    GPIOC->PUPDR &= ~(1 << 6); //PC3 clear
    GPIOC->PUPDR &= ~(1 << 7);
    GPIOC->PUPDR |= (1 << 4); //PC2 set
    GPIOC->PUPDR &= ~(1 << 5);
    GPIOC->PUPDR |= (1 << 6); //PC3 set
    GPIOC->PUPDR &= ~(1 << 7);
}