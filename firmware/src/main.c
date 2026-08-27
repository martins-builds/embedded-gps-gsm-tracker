#include "gps_task.h"
#include "gsm_task.h"
#include "clock.h"
#include "gpio.h"
#include "display_task.h"
#include "rtc.h"
#include "power.h"
#include "FreeRTOS.h"

__attribute__((used)) void main(void){
    clock_init();
    gpio_init();
    button_interrupt_init();
    i2c_init();
    uart1_init();
    uart3_init();

}