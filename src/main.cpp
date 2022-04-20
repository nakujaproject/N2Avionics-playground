#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */
#include "../include/functions.h"
#include "../include/functions.h"
#include "../include/kalmanfilter.h"
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/readsensors.h"
#include "../include/transmitlora.h"
#include "../include/defs.h"
#include <SPI.h>

static const BaseType_t pro_cpu = 0;

static const BaseType_t app_cpu = 1;

TaskHandle_t LoRaTelemetryTaskHandle;

TaskHandle_t ReadTelemetryTaskHandle;

TaskHandle_t SDLogTelemetryTaskHandle;

TaskHandle_t FlightControlTaskHandle;

static uint8_t telemetry_queue_length = 10;
static QueueHandle_t telemetry_queue;

static int state = 0;

// uninitalised pointers to SPI objects
SPIClass *hspi = NULL;

struct LogData dummyData()
{
    struct LogData ld;

    if (state == 5)
    {
        ld.latitude = 52.3;
        ld.longitude = 582.5;
    }
    ld.altitude = 3;

    return ld;
}
struct LogData readData()
{
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct LogData ld;
    struct GPSReadings gpsReadings;

    readings = get_readings();

    // TODO: very important to know the orientation of the altimeter
    filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    // TODO: why is the counter being passed here?
    state = checkState(filtered_values.displacement, filtered_values.velocity, 0, state);

    // if we reach ground state we can start reading gpsData
    if (state == 5)
    {
        gpsReadings = get_gps_readings();
    }

    ld = formart_data(readings, filtered_values, gpsReadings);
    ld.state = state;

    return ld;
}

void LoRaTelemetryTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {

        // Ticks to wait has been optimised
        if (xQueueReceive(telemetry_queue, (void *)&ld, 10) == pdTRUE)
        {
            sendTelemetryLora(ld);
        }

        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void ReadTelemetryTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {

        // ld = readData();
        ld = dummyData();
        if (xQueueSend(telemetry_queue, (void *)&ld, 0) != pdTRUE)
        {
            Serial.println("Queue Full!");
        }

        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void SDLogTelemetryTask(void *parameter)
{
    struct LogData ld;
    for (;;)
    {

        if (xQueueReceive(telemetry_queue, (void *)&ld, 0) == pdTRUE)
        {
            const char *message = printTelemetryMessage(ld);

            // appendToFile(message, telemetryLogFile);
        }

        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
// void FlightControlTask(void *parameter)
// {
//     // TODO: implement flight control
//     for (;;)
//     {
//     }
//     // yield to another task
//     vTaskDelay(100 / portTICK_PERIOD_MS);
// }

void setup()
{

    Serial.begin(115200);

    // set up slave select pins as outputs as the Arduino API
    pinMode(LORA_CS_PIN, OUTPUT);   // HSPI SS for use by LORA
    pinMode(SDCARD_CS_PIN, OUTPUT); // VSPI SS for use by SDCARD

    // set up parachute pin
    pinMode(EJECTION_PIN, OUTPUT);

    hspi = new SPIClass(HSPI);
    hspi->begin();

    SPIClass &spi = *hspi;
    setUpLoraOnBoard(spi);

    init_components();

    // get the base_altitude
    // SensorReadings readings = get_readings();
    // FilteredValues filtered_values = kalmanUpdate(readings.altitude, readings.ay);
    // base_altitude = filtered_values.displacement;

    telemetry_queue = xQueueCreate(telemetry_queue_length, sizeof(LogData));

    // initialize core tasks
    xTaskCreatePinnedToCore(LoRaTelemetryTask, "LoRaTelemetryTask", 10000, NULL, 1, &LoRaTelemetryTaskHandle, pro_cpu);

    xTaskCreatePinnedToCore(ReadTelemetryTask, "ReadTelemetryTask", 10000, NULL, 1, &ReadTelemetryTaskHandle, app_cpu);

    xTaskCreatePinnedToCore(SDLogTelemetryTask, "SDLogTelemetryTask", 10000, NULL, 1, &SDLogTelemetryTaskHandle, app_cpu);

    // xTaskCreatePinnedToCore(FlightControlTask, "FlightControlTask", 10000, NULL, 1, &FlightControlTaskHandle, tskNO_AFFINITY);

    // Delete "setup and loop" task

    vTaskDelete(NULL);
}

void loop() {}
