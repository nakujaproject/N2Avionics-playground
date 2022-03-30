
#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */

#include "../include/functions.h"
#include "../include/kalmanfilter.h"
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/transmitwifi.h"
#include "../include/readsensors.h"
#include "../include/transmitlora.h"
#include "../include/defs.h"

int sentCounter = 1, messageLostCounter = 0;

void setup()
{
    // init_components();
    Serial.begin(BAUD_RATE);
    setUpLoraOnBoard();
}

void loop()
{
    // struct LogData ld = readData();
    struct LogData ld = dummyData();

    char message[256];
    sprintf(message, "{\"Counter\":%d,\"Altitude\":%.3f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,\"s\":%.3f,\"v\":%.3f,\"a\":%.3f,\"Current State\":%d,\"Longitude\":%.3f,\"Latitude\":%.3f\n}", ld.counter, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.state, ld.longitude, ld.latitude);

    sendMessageLora(message, sentCounter);

    sentCounter++;
}
