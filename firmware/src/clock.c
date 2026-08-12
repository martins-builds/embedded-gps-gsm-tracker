#include "clock.h"

void clock_init(void){
    RCC->CR |= (1 << 16);
    while(RCC->CR & (1 << 17));
    FLASH->ACR |= (4 << 0);
    RCC->PLLCFGR = (1 << 4) | (20 << 8) | (2 << 25);
    RCC->CR |= (1 << 24);
    while(RCC->CR & (1 << 25));
    RCC->CFGR |= (3 << 0);
    while(RCC->CR & (3 << 2));
}