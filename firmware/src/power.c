#include "power.h"

void enter_stop_mode(void){
    PWR->CR1 &= ~(3 << 0);
    PWR->CR1 |= (2 << 0); //using stop 2 mode

    SCB->SCR |= (1 << 2); //sleepdeep

    /*hard to find, keep sleeping till interrupt i.e power on
    or distress signal*/
    __asm volatile ("wfi"); 
    
}