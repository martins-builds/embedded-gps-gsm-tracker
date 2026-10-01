#ifndef RTC_H
#define RTC_H

#include "stm32l476re.h"

void rtc_init(void);
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *day);

#endif