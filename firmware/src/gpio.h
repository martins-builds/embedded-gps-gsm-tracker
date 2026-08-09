#ifndef GPIO_H
#define GPIO_H

#include "stm32l476re.h"

void gpio_init(void);

void power_btn_pressed(void);
void distress_btn_pressed(void);

void EXTI3_IRQHandler(void);
void EXTI2_IRQHandler(void);

#endif