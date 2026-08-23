#include "gps_task.h"
#include "gsm_task.h"
#include "power.h"
#include <stdio.h>
#include <string.h>

__attribute__((used)) void main(void){
    while (1){
        gsm_task(&gps_data);
        enter_stop_mode();
    }
    
    return 0;
}