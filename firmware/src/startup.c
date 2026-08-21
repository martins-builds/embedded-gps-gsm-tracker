#include <stdint.h>
#include "stm32l476re.h"

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

void main(void);
void Reset_Handler(void);
void Default_Handler(void);
void rtc_init(void);

/* FreeRTOS handlers (see FreeRTOSConfig.h for the #define mapping) */
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;
    main();
    while (1) {}
}

void Default_Handler(void) { while (1) {} }

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
}
/* STM32L476RE vector table.
 * Positions/names per RM0351 + CMSIS device header (stm32l476xx.h).
 * DIFFERENT from F446 -- do not reuse the old table.
 *
 * Peripherals you'll actually use for the tracker are labeled inline:
 *   EXTI2  -> BTN_INT_EX2 (SOS button)
 *   EXTI3  -> BTN_PWR_EX3 (power/display button)
 *   USART1 -> GPS module
 *   USART3 -> GSM/SIM800L module
 *   I2C1   -> OLED (SSD1306)
 *   RTC_*  -> hardware RTC / wakeup
 * Everything else defaults to Default_Handler until you need it --
 * fill in real ISRs as you build each driver.
 */
__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void*)&_estack,           // 0   Initial stack pointer
    Reset_Handler,              // 1   Reset
    Default_Handler,            // 2   NMI
    Default_Handler,            // 3   HardFault
    Default_Handler,            // 4   MemManage
    Default_Handler,            // 5   BusFault
    Default_Handler,            // 6   UsageFault
    Default_Handler,            // 7   Reserved
    Default_Handler,            // 8   Reserved
    Default_Handler,            // 9   Reserved
    Default_Handler,            // 10  Reserved
    SVC_Handler,                 // 11  SVCall            (FreeRTOS)
    Default_Handler,            // 12  DebugMon
    Default_Handler,            // 13  Reserved
    PendSV_Handler,              // 14  PendSV            (FreeRTOS)
    SysTick_Handler,             // 15  SysTick           (FreeRTOS)

    /* ---- External interrupts (IRQn 0 onward) ---- */
    Default_Handler,            // 16  IRQ0  WWDG
    Default_Handler,            // 17  IRQ1  PVD_PVM
    Default_Handler,            // 18  IRQ2  TAMP_STAMP
    Default_Handler,            // 19  IRQ3  RTC_WKUP
    Default_Handler,            // 20  IRQ4  FLASH
    Default_Handler,            // 21  IRQ5  RCC
    Default_Handler,            // 22  IRQ6  EXTI0
    Default_Handler,            // 23  IRQ7  EXTI1
    Default_Handler,            // 24  IRQ8  EXTI2         -> BTN_INT_EX2 (SOS button)
    Default_Handler,            // 25  IRQ9  EXTI3         -> BTN_PWR_EX3 (power button)
    Default_Handler,            // 26  IRQ10 EXTI4
    Default_Handler,            // 27  IRQ11 DMA1_Channel1
    Default_Handler,            // 28  IRQ12 DMA1_Channel2
    Default_Handler,            // 29  IRQ13 DMA1_Channel3
    Default_Handler,            // 30  IRQ14 DMA1_Channel4
    Default_Handler,            // 31  IRQ15 DMA1_Channel5
    Default_Handler,            // 32  IRQ16 DMA1_Channel6
    Default_Handler,            // 33  IRQ17 DMA1_Channel7
    Default_Handler,            // 34  IRQ18 ADC1_2
    Default_Handler,            // 35  IRQ19 CAN1_TX
    Default_Handler,            // 36  IRQ20 CAN1_RX0
    Default_Handler,            // 37  IRQ21 CAN1_RX1
    Default_Handler,            // 38  IRQ22 CAN1_SCE
    Default_Handler,            // 39  IRQ23 EXTI9_5
    Default_Handler,            // 40  IRQ24 TIM1_BRK_TIM15
    Default_Handler,            // 41  IRQ25 TIM1_UP_TIM16
    Default_Handler,            // 42  IRQ26 TIM1_TRG_COM_TIM17
    Default_Handler,            // 43  IRQ27 TIM1_CC
    Default_Handler,            // 44  IRQ28 TIM2
    Default_Handler,            // 45  IRQ29 TIM3
    Default_Handler,            // 46  IRQ30 TIM4
    Default_Handler,            // 47  IRQ31 I2C1_EV        -> OLED (SSD1306)
    Default_Handler,            // 48  IRQ32 I2C1_ER
    Default_Handler,            // 49  IRQ33 I2C2_EV
    Default_Handler,            // 50  IRQ34 I2C2_ER
    Default_Handler,            // 51  IRQ35 SPI1
    Default_Handler,            // 52  IRQ36 SPI2
    Default_Handler,            // 53  IRQ37 USART1        -> GPS module
    Default_Handler,            // 54  IRQ38 USART2
    Default_Handler,            // 55  IRQ39 USART3        -> GSM/SIM800L module
    Default_Handler,            // 56  IRQ40 EXTI15_10
    Default_Handler,            // 57  IRQ41 RTC_Alarm
    Default_Handler,            // 58  IRQ42 DFSDM1_FLT3
    Default_Handler,            // 59  IRQ43 TIM8_BRK
    Default_Handler,            // 60  IRQ44 TIM8_UP
    Default_Handler,            // 61  IRQ45 TIM8_TRG_COM
    Default_Handler,            // 62  IRQ46 TIM8_CC
    Default_Handler,            // 63  IRQ47 ADC3
    Default_Handler,            // 64  IRQ48 FMC
    Default_Handler,            // 65  IRQ49 SDMMC1
    Default_Handler,            // 66  IRQ50 TIM5
    Default_Handler,            // 67  IRQ51 SPI3
    Default_Handler,            // 68  IRQ52 UART4
    Default_Handler,            // 69  IRQ53 UART5
    Default_Handler,            // 70  IRQ54 TIM6_DACUNDER
    Default_Handler,            // 71  IRQ55 TIM7
    Default_Handler,            // 72  IRQ56 DMA2_Channel1
    Default_Handler,            // 73  IRQ57 DMA2_Channel2
    Default_Handler,            // 74  IRQ58 DMA2_Channel3
    Default_Handler,            // 75  IRQ59 DMA2_Channel4
    Default_Handler,            // 76  IRQ60 DMA2_Channel5
    Default_Handler,            // 77  IRQ61 DFSDM1_FLT0
    Default_Handler,            // 78  IRQ62 DFSDM1_FLT1
    Default_Handler,            // 79  IRQ63 DFSDM1_FLT2
    Default_Handler,            // 80  IRQ64 COMP
    Default_Handler,            // 81  IRQ65 LPTIM1
    Default_Handler,            // 82  IRQ66 LPTIM2
    Default_Handler,            // 83  IRQ67 OTG_FS
    Default_Handler,            // 84  IRQ68 DMA2_Channel6
    Default_Handler,            // 85  IRQ69 DMA2_Channel7
    Default_Handler,            // 86  IRQ70 LPUART1       -> works in Stop mode, useful later
    Default_Handler,            // 87  IRQ71 QUADSPI
    Default_Handler,            // 88  IRQ72 I2C3_EV
    Default_Handler,            // 89  IRQ73 I2C3_ER
    Default_Handler,            // 90  IRQ74 SAI1
    Default_Handler,            // 91  IRQ75 SAI2
    Default_Handler,            // 92  IRQ76 SWPMI1
    Default_Handler,            // 93  IRQ77 TSC
    Default_Handler,            // 94  IRQ78 LCD
    Default_Handler,            // 95  IRQ79 AES
    Default_Handler,            // 96  IRQ80 RNG
    Default_Handler,            // 97  IRQ81 FPU
};
