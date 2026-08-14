#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "stm32l476re.h"
#include <stdint.h>

void i2c_init(void);
void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color);

#endif