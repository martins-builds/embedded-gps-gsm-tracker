/* Linker script for STM32L476RE
 * Flash: 512K @ 0x08000000
 * SRAM1: 96K  @ 0x20000000  (main RAM - stack, heap, .data, .bss)
 * SRAM2: 32K  @ 0x10000000  (separate address space - NOT contiguous with SRAM1)
 *
 * NOTE: SRAM2 is intentionally left unused here for simplicity.
 * If you need it later (e.g. for a DMA buffer region), add a second
 * memory region and an explicit output section targeting it.
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /* Reserve space for heap + stack, and catch overflow at link time */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        . = . + 0x400; /* minimum heap - FreeRTOS heap_4 has its own static array, this is just for any libc use */
        . = ALIGN(8);
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
