#ifndef STM32L476RE_H
#define STM32L476RE_H

#include <stdint.h>

//BASE ADDRESSES
#define PERIPH_BASE    0x40000000 
#define RCC_BASE       0x40021000
#define GPIOA_BASE     0x48000000
#define GPIOB_BASE     0x48000400
#define GPIOC_BASE     0x48000800
#define GPIOH_BASE     0x48001C00
#define SYSCFG_BASE    0x40010000
#define EXTI_BASE      0x40010400
#define FLASH_BASE     0x40022000
#define I2C1_BASE      0x40005400
#define USART1_BASE    0x40013800
#define USART3_BASE    0x40004800
#define PWR_BASE       0x40007000

//MEMORY MAPS
typedef struct 
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
    volatile uint32_t BRR;
    volatile uint32_t ASCR;
} GPIO_TypeDef;

typedef struct 
{
    volatile uint32_t CR;
    volatile uint32_t ICSCR;
    volatile uint32_t CFGR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t PLLSAI1CFGR;
    volatile uint32_t PLLSAI2CFGR;
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    volatile uint32_t RESERVED0;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    volatile uint32_t RESERVED1;
    volatile uint32_t APB1RSTR1;
    volatile uint32_t APB1RSTR2;
    volatile uint32_t APB2RSTR;
    volatile uint32_t RESERVED2;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    volatile uint32_t RESERVED3;
    volatile uint32_t APB1ENR1;
    volatile uint32_t APB1ENR2;
    volatile uint32_t APB2ENR;
    volatile uint32_t RESERVED4;
    volatile uint32_t AHB1SMENR;
    volatile uint32_t AHB2SMENR;
    volatile uint32_t AHB3SMENR;
    volatile uint32_t RESERVED5;
    volatile uint32_t APB1SMENR1;
    volatile uint32_t APB1SMENR2;
    volatile uint32_t APB2SMENR;
    volatile uint32_t CCIPR;
    volatile uint32_t RESERVED6;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t CRRCR;
    volatile uint32_t CCIPR2;
} RCC_TypeDef;

typedef struct
{
    volatile uint32_t MEMRMP;
    volatile uint32_t CFGR1;
    volatile uint32_t EXTICR1;
    volatile uint32_t EXTICR2;
    volatile uint32_t EXTICR3;
    volatile uint32_t EXTICR4;
    volatile uint32_t SCSR;
    volatile uint32_t CFGR2;
    volatile uint32_t SWPR;
    volatile uint32_t SKR;
    volatile uint32_t SWPR2;
} SYSCFG_TypeDef;

typedef struct
{
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t PR1;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t PR2;
} EXTI_TypeDef;

typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t PDKEYR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t ECCR;
    volatile uint32_t RESERVED0;
    volatile uint32_t OPTR;
    volatile uint32_t PCROP1SR;
    volatile uint32_t PCROP1ER;
    volatile uint32_t WRP1AR;
    volatile uint32_t WRP1BR;
    volatile uint32_t RESERVED1[4];
    volatile uint32_t PCROP2SR;
    volatile uint32_t PCROP2ER;
    volatile uint32_t WRP2AR;
    volatile uint32_t WRP2BR;
} FLASH_TypeDef;

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t TIMINGR;
    volatile uint32_t TIMEOUTR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t PECR;
    volatile uint32_t RXDR;
    volatile uint32_t TXDR;
} I2C_TypeDef;

typedef struct 
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
} USART_TypeDef; //what about LPUART

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t CR4;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t SCR;
    volatile uint32_t RESERVED0;
    volatile uint32_t PUCRA;
    volatile uint32_t PDCRA;
    volatile uint32_t PUCRB;
    volatile uint32_t PDCRB;
    volatile uint32_t PUCRC;
    volatile uint32_t PDCRC;
    volatile uint32_t PUCRD;
    volatile uint32_t PDCRD;
    volatile uint32_t PUCRE;
    volatile uint32_t PDCRE;
    volatile uint32_t PUCRF;
    volatile uint32_t PDCRF;
    volatile uint32_t PUCRG;
    volatile uint32_t PDCRG;
    volatile uint32_t PUCRH;
    volatile uint32_t PDCRH;
    volatile uint32_t PUCRI;
    volatile uint32_t PDCRI;
} PWR_TypeDef;

//POINTER MACROS
#define GPIOA  ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB  ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC  ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOH  ((GPIO_TypeDef *) GPIOH_BASE)

#define RCC    ((RCC_TypeDef *) RCC_BASE)

#define SYSCFG ((SYSCFG_TypeDef*) SYSCFG_BASE)
#define EXTI   ((EXTI_TypeDef*) EXTI_BASE)
#define FLASH  ((FLASH_TypeDef*) FLASH_BASE)

#define I2C    ((I2C_TypeDef*) I2C1_BASE)
#define USART1  ((USART_TypeDef*) USART1_BASE)
#define USART3  ((USART_TypeDef*) USART3_BASE)
#define PWR    ((PWR_TypeDef*) PWR_BASE)

#endif