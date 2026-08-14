#include "display_task.h"
#include <stdint.h>
#include <string.h>

uint8_t framebuffer[1024];

// List of initialization commands for a standard 128x64 OLED
const uint8_t SSD1306_Init_Sequence[] = {
    0xAE, // 1. Turn Display OFF
    
    0xD5, // 2. Set Display Clock Divide Ratio/Oscillator Frequency
    0x80, //    Suggested ratio (Default power-on value)
        
    0xA8, // 3. Set Multiplex Ratio
    0x3F, //    1/64 duty cycle (for 64-pixel height)
    
    0xD3, // 4. Set Display Offset
    0x00, //    No offset
        
    0x40, // 5. Set Display Start Line (0x40 to 0x7F) -> Line 0
        
    0x8D, // 6. Charge Pump Control
    0x14, //    Enable Charge Pump (Required for onboard step-up converter)
        
    0x20, // 7. Set Memory Addressing Mode
    0x00, //    0x00 = Horizontal Addressing Mode
        
    0xA1, // 8. Set Segment Re-map (0xA0/0xA1 to flip horizontal orientation)
    0xC8, // 9. Set COM Output Scan Direction (0xC0/0xC8 to flip vertical orientation)
        
    0xDA, // 10. Set COM Pins Hardware Configuration
    0x12, //     Alternative COM pin config (Sequential vs Alternative)
        
    0x81, // 11. Set Contrast Control
    0xCF, //     Brightness value (0x01 to 0xFF)
        
    0xD9, // 12. Set Pre-charge Period
    0xF1, //     Phase 1 = 1 DCLK, Phase 2 = 15 DCLK
        
    0xDB, // 13. Set VCOMH Deselect Level
    0x40, //     ~0.83 x VCC
        
    0xA4, // 14. Entire Display ON (Resume to RAM content)
    0xA6, // 15. Set Normal/Inverse Display (0xA6 = Normal, 0xA7 = Inverse)
        
    0xAF  // 16. Turn Display ON
};

void i2c_init(void){
    // alternate function modes
    GPIOB->MODER |= ((1 << 13) | (1 << 15)); 
    GPIOB->MODER &= ~((1 << 12) | (1 << 14));

    GPIOB->OTYPER |= (1 << 6); //open drain output
    GPIOB->OTYPER |= (1 << 7);

    GPIOB->AFRL &= ~((1 << 24) | (1 << 28)); //clear AF field
    GPIOB->AFRL |=  ((4 << 24) | (4 << 28)); //set AF fields     // AF4 for PB6

    RCC->APB1ENR1 |= (1 << 21); // enable 12c1 for display

    I2C1->TIMINGR = 0x4403F4B;
    I2C1->CR1 |= (1 << 0); //periferal enable
}
void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) {
        return; // Boundary check to prevent writing outside the array
    }

    // Determine the exact byte index inside the 1024 element array
    // y / 8 identifies the vertical Page; multiplying by 128 moves to that page block.
    uint16_t index = x + ((y / 8) * 128);

    // Calculate bit offset inside the targeted byte (y % 8)
    if (color) {
        framebuffer[index] |= (1 << (y % 8));  // Force bit HIGH
    } else {
        framebuffer[index] &= ~(1 << (y % 8)); // Force bit LOW
    }
}
void i2c_write(uint8_t dev_addr, uint8_t *data, uint16_t len){
    I2C1->CR2 |= (0x3C << 0);
    //loop to txdr while warching flag
    I2C1->CR2 |= (1 << 25); //autoend
}