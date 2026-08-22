#include "power.h"

void enter_stop_mode(void){
    PWR->CR1 &= ~(3 << 0);
    PWR->CR1 |= (2 << 0); //using stop 2 mode
}