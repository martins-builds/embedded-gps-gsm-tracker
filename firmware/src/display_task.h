#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "stm32l476re.h"
#include "font8x8.h"
#include <stdint.h>

void i2c_init(void);
void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color);
void OLED_DrawChar(x, y, char c);
void OLED_DrawString(x, y, char *str);
void i2c_write(uint8_t dev_addr, uint8_t *data, uint16_t len);
void ssd1306_init(void);
void ssd1306_update_display(void);
void display_task(void *pvParameters);

#endif