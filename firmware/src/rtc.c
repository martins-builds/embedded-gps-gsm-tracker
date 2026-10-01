#include "rtc.h"

void rtc_init(void){
    RCC->APB1ENR1 |= (1 << 28); // power enabled
    PWR->CR1 |= (1 << 8); //backup domain
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
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *day){
    uint8_t YT = (RTC->DR >> 20) & 0xF;   // bits [23:20], 4 bits wide
    uint8_t YU = (RTC->DR >> 16) & 0xF;   // bits [19:16], 4 bits wide
    uint8_t MT = (RTC->DR >> 12) & 0x1;   // bit [12], 1 bit wide
    uint8_t MU = (RTC->DR >> 8)  & 0xF;   // bits [11:8], 4 bits wide
    uint8_t DT = (RTC->DR >> 4)  & 0x3;   // bits [5:4], 2 bits wide
    uint8_t DU = (RTC->DR >> 0)  & 0xF;   // bits [3:0], 4 bits wide

    uint8_t y = YT*10 + YU;
    uint8_t m = MT*10 + MU;
    uint8_t d = DT*10 + DU;
}