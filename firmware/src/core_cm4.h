#ifndef CORE_CM4_H
#define CORE_CM4_H

#include <stdint.h>

#define NVIC_BASE   0xE000E100
#define SCB_BASE    0xE000ED00

//hardest struct yet
typedef struct {
    volatile uint32_t ISER[8];    // 0x000 - 0x01C  Set-Enable
         uint32_t RESERVED0[24];  // 0x020 - 0x07C  padding to next block
    volatile uint32_t ICER[8];    // 0x080 - 0x09C  Clear-Enable
         uint32_t RESERVED1[24];  // 0x0A0 - 0x0FC
    volatile uint32_t ISPR[8];    // 0x100 - 0x11C  Set-Pending
         uint32_t RESERVED2[24];  // 0x120 - 0x17C
    volatile uint32_t ICPR[8];    // 0x180 - 0x19C  Clear-Pending
         uint32_t RESERVED3[24];  // 0x1A0 - 0x1FC
    volatile uint32_t IABR[8];    // 0x200 - 0x21C  Active Bit
         uint32_t RESERVED4[56];  // 0x220 - 0x2FC
    volatile uint8_t  IP[240];    // 0x300 - 0x3EC  Priority (byte-addressable)
         uint32_t RESERVED5[644];
    volatile uint32_t STIR;       // 0xE00  Software Trigger
} NVIC_TypeDef;

typedef struct {
     volatile uint32_t CPUID;
     volatile uint32_t ICSR;
     volatile uint32_t VTOR;
     volatile uint32_t AIRCR;
     volatile uint32_t SCR;
     volatile uint32_t CCR;
     volatile uint32_t SHPR1;
     volatile uint32_t SHPR2;
     volatile uint32_t SHPR3;
     volatile uint32_t SHCRS;
     volatile uint32_t CFSR;
     volatile uint32_t HFSR;
     volatile uint32_t MMAR;
     volatile uint32_t BFAR;
     volatile uint32_t AFSR;
} SCB_TypeDef;

#define NVIC   ((NVIC_TypeDef*) NVIC_BASE)

#endif