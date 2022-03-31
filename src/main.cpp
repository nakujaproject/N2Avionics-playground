
#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */

#include "../include/functions.h"
#include "../include/kalmanfilter.h"
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/readsensors.h"
#include "../include/transmitlora.h"
#include "../include/defs.h"


int counter = 0;

struct LogData readData()
{
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct GPSReadings gpsReadings;
    struct LogData ld;
    
    readings = get_readings();
    gpsReadings = get_gps_readings();

    filtered_values = kalmanUpdate(readings.altitude, readings.az);
    state = checkState(filtered_values.displacement, filtered_values.velocity, counter, state);
    
    ld = formart_data(readings, filtered_values,gpsReadings);
    ld.state = state;

    return ld;
}

void setup()
{
    init_components();
    Serial.begin(BAUD_RATE);
    pinMode(EJECTION_PIN, OUTPUT);
    setUpLoraOnBoard();
}

void loop()
{
    struct LogData ld = readData();
    ld.counter = counter;
    sendMessageLora(ld);
    counter++;
}

