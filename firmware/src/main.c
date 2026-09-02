#include "task.h"
#include "gps_task.h"
#include "gsm_task.h"
#include "clock.h"
#include "gpio.h"
#include "display_task.h"
#include "rtc.h"
#include "power.h"
#include "FreeRTOS.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for(;;);
}

__attribute__((used)) void main(void){
    //initializations
    clock_init();
    gpio_init();
    button_interrupt_init();
    i2c_init();
    uart1_init();
    uart3_init();
    rtc_init();
    gsm_task_init();
    ssd1306_init();
    
    //create tasks
    xTaskCreate(gsm_task, "GSM", 256, NULL, 1, NULL);
    xTaskCreate(gps_task, "GPM", 128, NULL, 1, NULL);
    xTaskCreate(display_task, "DISPLAY", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    //need watchdog for program break
}