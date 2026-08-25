#include "gps_task.h"
#include "gsm_task.h"
#include "power.h"
#include <stdio.h>
#include <string.h>

__attribute__((used)) void main(void){
    while (1){
        if (gps_data.valid){
            gsm_task(&gps_data);
        }
    }
    return 0;
}