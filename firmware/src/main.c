#include "gps_task.h"
#include <stdio.h>
#include <string.h>

int main(void){
    // Simulate what the ISR would have built, byte by byte, from a real sentence
    const char *fake_incoming = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W";
    strcpy(gps_rx_buffer, fake_incoming);
    gps_rx_buffer[strlen(fake_incoming)] = '\0';
    gps_line_ready = 1;   // pretend the ISR just saw '\n' and set this

    gps_process();

    printf("Valid: %d\n", gps_data.valid);
    printf("Time: %02d:%02d:%02d\n", gps_data.hours, gps_data.minutes, gps_data.seconds);
    printf("Lat: %.4f, Lon: %.4f\n", gps_data.latitude, gps_data.longitude);
    printf("gps_line_ready after processing: %d (should be 0)\n", gps_line_ready);

    return 0;
}