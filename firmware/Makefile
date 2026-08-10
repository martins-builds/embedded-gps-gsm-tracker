TARGET = main
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

FREERTOS = lib/FreeRTOS

C_SOURCES = src/main.c \
            src/startup.c \
            src/gps_task.c \
            src/gsm_task.c \
            src/display_task.c \
            src/gpio.c \
            src/clock.c \
            $(FREERTOS)/tasks.c \
            $(FREERTOS)/queue.c \
            $(FREERTOS)/list.c \
            $(FREERTOS)/timers.c \
            $(FREERTOS)/stream_buffer.c \
            $(FREERTOS)/portable/GCC/ARM_CM4F/port.c \
            $(FREERTOS)/portable/MemMang/heap_4.c

INCLUDES = -Isrc \
           -I$(FREERTOS)/include \
           -I$(FREERTOS)/portable/GCC/ARM_CM4F

# STM32L476 is Cortex-M4F, same core flags as F446 -- these don't change
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
         -O0 -g -Wall \
         $(INCLUDES)

# Linker script updated to the L476-specific one (correct RAM size/split)
LDFLAGS = -T STM32L476RE.ld \
          -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          --specs=nosys.specs -nostartfiles \
          -Wl,--gc-sections \
          -Wl,-Map=$(TARGET).map

all: $(TARGET).bin

$(TARGET).elf: $(C_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f *.elf *.bin *.map

.PHONY: all flash clean
