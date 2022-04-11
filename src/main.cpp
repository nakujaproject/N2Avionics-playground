
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
#include <SPI.h>

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

// uninitalised pointers to SPI objects
SPIClass *hspi = NULL;

int counter = 0;
struct LogData ld;

struct LogData readData()
{
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct GPSReadings gpsReadings;
    struct LogData ld;

    readings = get_readings();
    gpsReadings = get_gps_readings();

    filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    state = checkState(filtered_values.displacement, filtered_values.velocity, counter, state);

    ld = formart_data(readings, filtered_values, gpsReadings);
    ld.state = state;

    return ld;
}

void setup()
{
    // Initialize HSPI to be used by LORA according to https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/examples/SPI_Multiple_Buses/SPI_Multiple_Buses.ino
    Serial.begin(115200);
    hspi = new SPIClass(HSPI);
    hspi->begin();

    // // set up slave select pins as outputs as the Arduino API
    pinMode(LORA_CS_PIN, OUTPUT); // HSPI SS for use by LORA
    // pinMode(SDCARD_CS_PIN, OUTPUT); // VSPI SS for use by SDCARD

    // init_components();
    pinMode(33, OUTPUT);

    SPIClass &spi = *hspi;
    setUpLoraOnBoard(spi);
}

void loop()
{
    digitalWrite(33, !digitalRead(33));

    ld = readData();
    ld.counter = counter;
    sendMessageLora(ld);
    counter++;
}
