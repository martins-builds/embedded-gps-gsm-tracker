#include "rtc.h"

void rtc_init(void){
    PWR->CR1 |= (1 << 8);
    RCC->BDCR |= (1 << 0); //LSEON
    while (!(RCC->BDCR & (1 << 1))); //LSERDY
    RCC->BDCR |= (1 << 8); // select LSE
    RCC->BDCR |= (1 << 15); //enable clock
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    //enter initialization mode
    RTC->ISR |= (1 << 7);         // INIT bit
    while (!(RTC->ISR & (1 << 6)));  // wait for INITF
    //Program prescaler for 1Hz (LSE 32.768kHz -> async /128, sync /256)
    RTC->PRER = (127 << 16) | (255 << 0);   // PREDIV_A=127, PREDIV_S=255
    //Set 24-hour format
    RTC->CR &= ~(1 << 6);         // FMT = 0 (24-hour)
    //Exit initialization mode
    RTC->ISR &= ~(1 << 7);        // clear INIT bit
    //Re-enable write protection
    RTC->WPR = 0xFF; // write protection re-locks automatically on any non-key write
}