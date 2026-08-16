#include "gps_task.h"
#include <stdio.h>

int main(void) {
    const char *test_sentence = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    GPS_Data_t result;

    gps_parse_gprmc(test_sentence, &result);

    printf("Lat: %.4f, Lon: %.4f\n", result.latitude, result.longitude);
    // etc.
}