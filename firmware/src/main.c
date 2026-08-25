#include "gps_task.h"
#include "gsm_task.h"
#include "power.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include <string.h>

__attribute__((used)) void main(void){
    while (1){
        if (gps_data.valid){
            gsm_send_location(&gps_data);
        }
        enter_stop_mode();// not gonna use this for now
    }
}