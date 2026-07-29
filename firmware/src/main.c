#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//global variable
QueueHandle_t xAdcQueue;
QueueHandle_t xRfidQueue;
SemaphoreHandle_t xUartMutex;
volatile uint16_t adc_dma_value = 0;

//REGISTER BASES
#define RCC_BASE     0x40023800
#define GPIOA_BASE   0x40020000
#define USART2_BASE  0x40004400
#define I2C_BASE     0x40005400
#define GPIOB_BASE   0x40020400
#define LCD_ADDR     0x27
#define LCD_BL       0x08  // backlight bit
#define ADC1_BASE     0x40012000
#define ADC_COMMON_BASE  0x40012300   // ADC1_BASE + 0x300
#define DMA2_BASE    0x40026400
#define SPI1_BASE    0x40013000


//PINS
//port A
#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))
#define GPIOA_AFRL   (*(volatile uint32_t*)(GPIOA_BASE + 0x20))
#define GPIOA_OTYPER (*(volatile uint32_t*)(GPIOA_BASE + 0x04))

//port B
#define GPIOB_MODER  (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER (*(volatile uint32_t*)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR (*(volatile uint32_t*)(GPIOB_BASE + 0x08))
#define GPIOB_AFRH   (*(volatile uint32_t*)(GPIOB_BASE + 0x24))

//USART
#define RCC_APB1ENR  (*(volatile uint32_t*)(RCC_BASE    + 0x40))
#define USART_SR     (*(volatile uint32_t*)(USART2_BASE + 0x00))
#define USART_DR     (*(volatile uint32_t*)(USART2_BASE + 0x04))
#define USART_CR1    (*(volatile uint32_t*)(USART2_BASE + 0x0C))
#define USART_BRR    (*(volatile uint32_t*)(USART2_BASE + 0x08))

//PLL
#define RCC_CR       (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_PLLCFGR  (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_CFGR     (*(volatile uint32_t*)(RCC_BASE + 0x08))
#define FLASH_ACR    (*(volatile uint32_t*)(0x40023C00 + 0x00))

//I2C
#define I2C_CR1    (*(volatile uint32_t*)(I2C_BASE + 0x00))
#define I2C_CR2    (*(volatile uint32_t*)(I2C_BASE + 0x04))
#define I2C_CCR    (*(volatile uint32_t*)(I2C_BASE + 0x1C))
#define I2C_TRISE  (*(volatile uint32_t*)(I2C_BASE + 0x20))
#define I2C_DR     (*(volatile uint32_t*)(I2C_BASE + 0x10))
#define I2C_SR1    (*(volatile uint32_t*)(I2C_BASE + 0x14))
#define I2C_SR2    (*(volatile uint32_t*)(I2C_BASE + 0x18))
#define I2C_OAR1   (*(volatile uint32_t*)(I2C_BASE + 0x08))

//ADC
#define RCC_APB2ENR   (*(volatile uint32_t*)(RCC_BASE  + 0x44))
#define ADC1_CR1      (*(volatile uint32_t*)(ADC1_BASE + 0x04))
#define ADC1_CR2      (*(volatile uint32_t*)(ADC1_BASE + 0x08))
#define ADC1_SQR3     (*(volatile uint32_t*)(ADC1_BASE + 0x34))
#define ADC1_DR       (*(volatile uint32_t*)(ADC1_BASE + 0x4C))
#define ADC1_SMPR2    (*(volatile uint32_t*)(ADC1_BASE + 0x10))
#define ADC1_SR       (*(volatile uint32_t*)(ADC1_BASE + 0x00))

#define ADC_CCR      (*(volatile uint32_t*)(ADC_COMMON_BASE + 0x04))

//DMA Stream 0
#define DMA2_S0CR    (*(volatile uint32_t*)(DMA2_BASE + 0x010))
#define DMA2_S0NDTR  (*(volatile uint32_t*)(DMA2_BASE + 0x014))
#define DMA2_S0PAR   (*(volatile uint32_t*)(DMA2_BASE + 0x018))
#define DMA2_M0AR    (*(volatile uint32_t*)(DMA2_BASE + 0x01C))
#define DMA2_LISR    (*(volatile uint32_t*)(DMA2_BASE + 0x000))
#define DMA2_HISR    (*(volatile uint32_t*)(DMA2_BASE + 0x004))

//SPI1 registers
#define SPI_CR1    (*(volatile uint32_t*)(SPI1_BASE + 0x00))
#define SPI_CR2    (*(volatile uint32_t*)(SPI1_BASE + 0x04))
#define SPI_SR     (*(volatile uint32_t*)(SPI1_BASE + 0x08))
#define SPI_DR     (*(volatile uint32_t*)(SPI1_BASE + 0x0C))

//DWT REGISTERS
#define DWT_CTRL   (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT (*(volatile uint32_t*)0xE0001004)
#define DEMCR      (*(volatile uint32_t*)0xE000EDFC)

void dwt_init(void){
    DEMCR |= (1 << 24);     // enable trace
    DWT_CTRL |= (1 << 0);   // enable cycle counter
    DWT_CYCCNT = 0;
}
void delay_us(uint32_t us){
    uint32_t start = DWT_CYCCNT;
    uint32_t cycles = us * 180;
    while((DWT_CYCCNT - start) < cycles);
}
void delay_ms_busy(uint32_t ms){
    for(uint32_t i = 0; i < ms; i++){
        delay_us(1000);
    }
}

//start of usart2
void usart_init(void){
    RCC_APB1ENR |= (1 << 17);  // USART2EN
    USART_BRR = 45000000 / 9600;  // = 4687
    USART_CR1 = (1 << 13) | (1 << 3) | (1 << 2);  // UE, TE, RE
}
void usart_send(char data){
    while(!(USART_SR & (1 << 7)));  // wait until TXE is set
    USART_DR = data;
}
char usart_receive(void){
    while(!(USART_SR & (1 << 5)));  // wait until RXNE is set
    return USART_DR;
}
void usart_print(const char *str){
    while (*str) {
        usart_send(*str++);
    }
}
void usart_print_number(uint16_t num){
    char buf[6];
    uint8_t i = 0;
    
    if (num == 0){
        usart_send('0');
        return;
    }
    while (num > 0){
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    // digits are reversed, print backwards
    while (i > 0){
        usart_send(buf[--i]);
    }
}
void usart_print_hex(uint8_t val){
    char hex[] = "0123456789ABCDEF";
    usart_send(hex[val >> 4]);
    usart_send(hex[val & 0x0F]);
}
//clock function
void clock_init(void){
    FLASH_ACR |= (5 << 0);
    FLASH_ACR |= (1 << 8) | (1 << 9) | (1 << 10);
    RCC_CR |= (1 << 16);
    while(!(RCC_CR & (1 << 17)));  // wait until bit 17 becomes 1
    RCC_PLLCFGR = (1 << 22) | (8 << 0) | (360 << 6) | (0 << 16);
    RCC_CR |= (1 << 24);
    while(!(RCC_CR & (1 << 25)));
    RCC_CFGR = (0 << 4) | (5 << 10) | (4 << 13);
    RCC_CFGR |= (2 << 0);
    while(!(RCC_CFGR & (2 << 2)));
}


//I2C function
void i2c_init(void){
    // GPIOB clock
    RCC_AHB1ENR |= (1 << 1);
    RCC_APB1ENR |= (1 << 21);

    // PB8 and PB9 alternate function mode (10)
    GPIOB_MODER |= (1 << 17) | (1 << 19);
    GPIOB_MODER &= ~((1 << 16) | (1 << 18));

    // open-drain
    GPIOB_OTYPER |= (1 << 8) | (1 << 9);

    // high speed
    GPIOB_OSPEEDR |= (1 << 17) | (1 << 16) | (1 << 19) | (1 << 18);

    GPIOB_AFRH &= ~(0xF << 0);   // clear PB8's 4 bits
    GPIOB_AFRH |=  (4   << 0);   // set AF4
    GPIOB_AFRH &= ~(0xF << 4);  // clear PB9's 4 bits
    GPIOB_AFRH |=  (4   << 4);  // set AF4

    I2C_CR2 = 45;
    I2C_CCR = 225;
    I2C_TRISE = 46;
    I2C_CR1 |= (1 << 0);
}

void i2c_start(void){
    I2C_CR1 |= (1 << 8);
    while(!(I2C_SR1 & (1 << 0)));
}
void i2c_write_addr(uint8_t addr){
    I2C_DR = (addr << 1);
    while(!(I2C_SR1 & (1 << 1)));
    volatile uint32_t tmp = I2C_SR1;
    tmp = I2C_SR2;
    (void)tmp;
}
void i2c_write_byte(uint8_t data){
    I2C_DR = data;
    while(!(I2C_SR1 & (1 << 7)));
}
void i2c_stop(void){
    while(!(I2C_SR1 & (1 << 2)));
    I2C_CR1 |= (1 << 9);
}
void pcf8574_write(uint8_t data){
    i2c_start();
    i2c_write_addr(LCD_ADDR);
    i2c_write_byte(data);
    i2c_stop();
}

//lcd functions
void lcd_send_byte(uint8_t data){
    i2c_start();
    i2c_write_addr(LCD_ADDR);
    i2c_write_byte(data | LCD_BL);
    i2c_stop();
}
void lcd_pulse_enable(uint8_t data){
    lcd_send_byte(data | 0x04);   // EN high
    delay_us(1);
    lcd_send_byte(data & ~0x04);  // EN low
    delay_us(1);
}
void lcd_send_nibble(uint8_t nibble, uint8_t rs){
    uint8_t data = (nibble & 0xF0) | LCD_BL | rs;
    lcd_pulse_enable(data);
}
void lcd_send_cmd(uint8_t cmd){
    lcd_send_nibble(cmd & 0xF0, 0x00);        // high nibble, RS=0
    lcd_send_nibble((cmd << 4) & 0xF0, 0x00); // low nibble, RS=0
}
void lcd_send_char(uint8_t ch){
    lcd_send_nibble(ch & 0xF0, 0x01);        // high nibble, RS=1
    lcd_send_nibble((ch << 4) & 0xF0, 0x01); // low nibble, RS=1
}
void lcd_init(void){
    delay_ms_busy(50);              // wait for LCD power up

    // special initialization sequence - 4-bit mode
    lcd_send_nibble(0x30, 0);   // function set
    delay_ms_busy(5);
    lcd_send_nibble(0x30, 0);   // function set
    delay_us(150);
    lcd_send_nibble(0x30, 0);   // function set
    delay_us(150);
    lcd_send_nibble(0x20, 0);   // switch to 4-bit mode

    // now in 4-bit mode - configure
    lcd_send_cmd(0x28);  // 2 lines, 5x8 font
    lcd_send_cmd(0x0C);  // display on, cursor off
    lcd_send_cmd(0x06);  // entry mode - increment cursor
    lcd_send_cmd(0x01);  // clear display
    delay_ms_busy(2);        // clear needs extra time
}
void lcd_print(const char *str){
    while(*str){
        lcd_send_char(*str++);
    }
}
void lcd_print_number(uint16_t num){
    char buf[6];
    uint8_t i = 0;
    if (num == 0){
        lcd_send_char('0');
        return;
    }
    while (num > 0){
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0){
        lcd_send_char(buf[--i]);
    }
}
void lcd_set_cursor(uint8_t row, uint8_t col){
    uint8_t addr = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_send_cmd(addr);
}

//ADC functions
void adc_init(void){
    //clocks
    RCC_APB2ENR |= (1 << 8);
    RCC_AHB1ENR |= (1 << 0);

    GPIOA_MODER |= (1 << 0) | (1 << 1);

    //12 bit resolution
    ADC1_CR1 &= ~(1 << 25);
    ADC1_CR1 &= ~(1 << 24);

    ADC1_SQR3 = 0;
    ADC1_SMPR2 |= (7 << 0);

    ADC1_CR2 |= (1 << 0);
    delay_ms_busy(1);
}

uint16_t adc_read(void){
    ADC1_CR2 |= (1 << 30);
    while(!(ADC1_SR & (1 << 1)));
    return ADC1_DR;
}

//dma functions
void dma_adc_init(void){
    RCC_AHB1ENR |= (1 << 22);

    DMA2_S0CR &= ~(1 << 0);//disable stream
    while(DMA2_S0CR & (1 << 0));   // wait for disable
    DMA2_S0PAR = (uint32_t)(ADC1_BASE + 0x4C);  // address of ADC1_DR
    DMA2_M0AR  = (uint32_t)&adc_dma_value;       // address of your variable
    DMA2_S0NDTR = 1;
    DMA2_S0CR |= (1 << 8) | (1 << 11) | (1 << 13);
    DMA2_S0CR |= (1 << 0);//enable stream

    ADC1_CR2 |= (1 << 1) | (1 << 8) | (1 << 9);  // CONT + DMA enable + DDS
}

//spi functions
void spi_init(void){
    //clocks
    RCC_APB2ENR |= (1 << 12);
    RCC_AHB1ENR |= (1 << 0);

    // PA5 bits 11:10 = 10
    GPIOA_MODER |= (1 << 11);
    GPIOA_MODER &= ~(1 << 10);
    // PA6 bits 13:12 = 10
    GPIOA_MODER |= (1 << 13);
    GPIOA_MODER &= ~(1 << 12);
    // PA7 bits 15:14 = 10
    GPIOA_MODER |= (1 << 15);
    GPIOA_MODER &= ~(1 << 14);
    // PA4 bits 9:8 = 01
    GPIOA_MODER &= ~(1 << 9);
    GPIOA_MODER |= (1 << 8);

    GPIOA_AFRL &= ~(0xF << 20); //clear PA5
    GPIOA_AFRL |= (5 << 20);  // PA5 = AF5
    GPIOA_AFRL &= ~(0xF << 24); //clear PA6
    GPIOA_AFRL |= (5 << 24);  // PA6 = AF5
    GPIOA_AFRL &= ~(0xF << 28); //clear PA7
    GPIOA_AFRL |= (5 << 28);  // PA7 = AF5

    GPIOA_ODR |= (1 << 4); // PA4 as output

    SPI_CR1 &= ~(1 << 0);//phase
    SPI_CR1 &= ~(1 << 1);//polarity
    SPI_CR1 |= (1 << 2);//master select
    SPI_CR1 |= (3 << 3);  // BR = 011 = /16 = 5.625MHz
    SPI_CR1 |= (1 << 8);//ssi
    SPI_CR1 |= (1 << 9);//ssm
    SPI_CR1 |= (1 << 6); //enable spi
}
uint8_t spi_transmit_receive(uint8_t data){
    SPI_DR = data;
    while(!(SPI_SR & (1 << 1)));
    while(!(SPI_SR & (1 << 0)));
    return SPI_DR;
}
void spi_cs_low(void){
    GPIOA_ODR &= ~(1 << 4); //low
}
void spi_cs_high(void){
    GPIOA_ODR |= (1 << 4); //high
}

//rc552 functions
void rc522_write_reg(uint8_t reg, uint8_t val){
    spi_cs_low();
    spi_transmit_receive((reg << 1) & 0x7E);
    spi_transmit_receive(val);
    spi_cs_high();
}
uint8_t rc522_read_reg(uint8_t reg){
    spi_cs_low();
    spi_transmit_receive(((reg << 1) & 0x7E) | 0x80);
    uint8_t result = spi_transmit_receive(0x00);
    spi_cs_high();
    return result;
}
void rc522_init(void){
    rc522_write_reg(0x01, 0x0F);
    delay_ms_busy(50);
    uint8_t val = rc522_read_reg(0x37);
    usart_print_number(val);
}
void rc522_antenna_on(void){
    uint8_t val = rc522_read_reg(0x14);
    if((val & 0x03) != 0x03){
        rc522_write_reg(0x14, val | 0x03);
    }
}
uint8_t rc522_transceive(uint8_t *send_data, uint8_t send_len, uint8_t *recv_data, uint8_t *recv_len){
    rc522_write_reg(0x04, 0x00);
    rc522_write_reg(0x0A, 0x80);
    rc522_write_reg(0x04, 0x7F);//clear interrupt flag
    for (uint8_t i = 0; i < send_len; i++)
    {
        rc522_write_reg(0x09, send_data[i]);//data to fifo
    }
    
    rc522_write_reg(0x0D, 0x00);
    rc522_write_reg(0x01, 0x0C);

    uint16_t timeout = 2000;
    while(!(rc522_read_reg(0x04) & ((1 << 5) | (1 << 0)))) {
        if(--timeout == 0) return 0;  // no card found
    }
    if(rc522_read_reg(0x04) & (1 << 0)) return 0;  // TimerIRq = timeout = no card

    // then read FIFO
    *recv_len = rc522_read_reg(0x0A);
    for(uint8_t i = 0; i < *recv_len; i++){
        recv_data[i] = rc522_read_reg(0x09);
    }
    return 1;  // card found
}
uint8_t rc522_request(void){
    rc522_write_reg(0x0D, 0x07);  // 7-bit frame for REQA
    uint8_t send = 0x26;
    uint8_t recv[2];
    uint8_t recv_len;
    return rc522_transceive(&send, 1, recv, &recv_len);
}
uint8_t rc522_anticollision(uint8_t *uid){
    rc522_write_reg(0x0D, 0x00);  // normal framing
    uint8_t send[2] = {0x93, 0x20};
    uint8_t recv[5];
    uint8_t recv_len;
    if(!rc522_transceive(send, 2, recv, &recv_len)) return 0;
    // copy 4 UID bytes
    for(uint8_t i = 0; i < 4; i++){
        uid[i] = recv[i];
    }
    return 1;
}
uint8_t rc522_read_card_uid(uint8_t *uid){
    if(!rc522_request()) return 0;
    return rc522_anticollision(uid);
}

void adc_task(void *pvParameters){
    while (1){
        uint16_t voltage_mv = (adc_read() * 3300UL) / 4095;
        xQueueSend(xAdcQueue, &voltage_mv, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
void rfid_task(void *pvParameters){
    while (1){
        uint8_t uid[4];
        if(rc522_read_card_uid(uid)){
            xQueueSend(xRfidQueue, &uid, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void display_task(void *pvParameter){
    while(1){
        uint16_t adcval;
        uint8_t uidval[4];
        xQueueReceive(xAdcQueue, &adcval, portMAX_DELAY);
        xSemaphoreTake(xUartMutex, portMAX_DELAY);
        usart_print("ADC: ");
        usart_print_number(adcval);
        usart_print("mV\r\n");
        // LCD row 2 — ADC level
        lcd_set_cursor(1, 0);
        lcd_print("Lvl: ");
        lcd_print_number(adcval);
        lcd_print("    ");  // pad with spaces to clear leftover digits

        if(xQueueReceive(xRfidQueue, uidval, 0) == pdTRUE){
            usart_print("UID: ");
            for(uint8_t i = 0; i < 4; i++){
                usart_print_hex(uidval[i]);
                usart_send(' ');
            }
            // LCD row 1 — UID
            lcd_set_cursor(0, 0);
            lcd_print("UID:");
            for(uint8_t i = 0; i < 4; i++){
                char hex[] = "0123456789ABCDEF";
                lcd_send_char(hex[uidval[i] >> 4]);
                lcd_send_char(hex[uidval[i] & 0x0F]);
                if(i < 3) lcd_send_char(' ');
            }
        }
        usart_print("\r\n");
        xSemaphoreGive(xUartMutex);
    }
}

__attribute__((used)) void main(void){
    clock_init();
    dwt_init();
    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // PA2 bits 5:4 = 10
    GPIOA_MODER |= (1 << 5);
    GPIOA_MODER &= ~(1 << 4);
    // PA3 bits 7:6 = 10
    GPIOA_MODER |= (1 << 7);
    GPIOA_MODER &= ~(1 << 6);

    GPIOA_AFRL |= (7 << 8);   // PA2 = AF7
    GPIOA_AFRL |= (7 << 12);  // PA3 = AF7

    usart_init();
    i2c_init();
    lcd_init();
    spi_init();
    rc522_init();
    rc522_antenna_on();
    adc_init();
    xUartMutex = xSemaphoreCreateMutex();
    xAdcQueue = xQueueCreate(10, sizeof(uint16_t));  // 10 items, each a uint16_t
    xRfidQueue = xQueueCreate(5, sizeof(uint8_t[4]));
    xTaskCreate(adc_task, "ADC", 256, NULL, 1, NULL);
    xTaskCreate(rfid_task, "RFID", 256, NULL, 1, NULL);
    xTaskCreate(display_task, "DISPLAY", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1);
}