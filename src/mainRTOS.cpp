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
TaskHandle_t LoRaGPSOnlyTaskHandle;

TaskHandle_t ReadTelemetryTaskHandle;
TaskHandle_t ReadGPSTaskHandle;

TaskHandle_t SDLogTelemetryTaskHandle;
TaskHandle_t SDLogGPSTaskHandle;

TaskHandle_t FlightControlTaskHandle;

static uint8_t telemetry_queue_length = 10;
static QueueHandle_t telemetry_queue;

static uint8_t gps_queue_length = 10;
static QueueHandle_t gps_queue;

// flag 1 means start sending GPS stop sending telemetry
static volatile uint8_t gps_flag = 0;

static volatile int state = 0;

// uninitalised pointers to SPI objects
SPIClass *hspi = NULL;

struct LogData readData()
{
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct LogData ld;

    readings = get_readings();

    // TODO: very important to know the orientation of the altimeter
    filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    // TODO: why is the counter being passed here?
    state = checkState(filtered_values.displacement, filtered_values.velocity, counter, state);

    // if we reach ground state we can start reading gpsData
    if (state == 5)
    {
        gps_flag = 1;
        vTaskDelete(LoRaTelemetryTaskHandle);
        vTaskDelete(ReadTelemetryTaskHandle);
        vTaskDelete(SDLogTelemetryTaskHandle);
    }

    ld = formart_data(readings, filtered_values);
    ld.state = state;

    return ld;
}

void LoRaTelemetryTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {
        if (gps_flag == 0)
        {
            if (xQueueReceive(telemetry_queue, (void *)&ld, 0) == pdTRUE)
            {
                sendTelemetryLora(ld);
            }
        }
        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void LoRaGPSOnlyTask(void *parameter)
{
    struct GPSReadings gpsReadings;
    for (;;)
    {
        if (gps_flag == 1)
        {
            if (xQueueReceive(gps_queue, (void *)&gpsReadings, 0) == pdTRUE)
            {
                sendGPSLora(gpsReadings);
            }
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
        if (gps_flag == 0)
        {
            ld = readData();
            if (xQueueSend(telemetry_queue, (void *)&ld, 0) != pdTRUE)
            {
                Serial.println("Queue Full!");
            }
        }
        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void ReadGPSOnlyTask(void *parameter)
{
    struct GPSReadings gpsReadings;
    for (;;)
    {
        if (gps_flag == 1)
        {

            gpsReadings = get_gps_readings();
            if (xQueueSend(gps_queue, (void *)&gpsReadings, 0) != pdTRUE)
            {
                Serial.println("Queue Full!");
            }
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
        if (gps_flag == 0)
        {
            if (xQueueReceive(telemetry_queue, (void *)&ld, 0) == pdTRUE)
            {
                const char *message = printTelemetryMessage(ld);
                appendToFile(message, telemetryLogFile);
            }
        }
        // yield to another task
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void SDLogGPSTask(void *parameter)
{
    struct GPSReadings gpsReadings;
    for (;;)
    {
        if (gps_flag == 1)
        {
            if (xQueueReceive(gps_queue, (void *)&gpsReadings, 0) == pdTRUE)
            {
                const char *message = printGPSMessage(gpsReadings);
                appendToFile(message, gpsLogFile);
            }
        }
    }
    // yield to another task
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void FlightControlTask(void *parameter)
{
    // TODO: implement flight control
    for (;;)
    {
    }
    // yield to another task
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

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
    SensorReadings readings = get_readings();
    FilteredValues filtered_values = kalmanUpdate(readings.altitude, readings.ay);
    base_altitude = filtered_values.displacement;

    telemetry_queue = xQueueCreate(telemetry_queue_length, sizeof(LogData));
    gps_queue = xQueueCreate(gps_queue_length, sizeof(GPSReadings));

    // initialize core tasks
    xTaskCreatePinnedToCore(LoRaTelemetryTask, "LoRaTelemetryTask", 10000, NULL, 1, &LoRaTelemetryTaskHandle, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(LoRaGPSOnlyTask, "LoRaGPSTask", 10000, NULL, 1, &LoRaGPSOnlyTaskHandle, tskNO_AFFINITY);

    xTaskCreatePinnedToCore(ReadTelemetryTask, "ReadTelemetryTask", 10000, NULL, 1, &ReadTelemetryTaskHandle, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(ReadGPSOnlyTask, "ReadGPSTask", 10000, NULL, 1, &ReadGPSTaskHandle, tskNO_AFFINITY);

    xTaskCreatePinnedToCore(SDLogTelemetryTask, "SDLogTelemetryTask", 10000, NULL, 1, &SDLogTelemetryTaskHandle, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(SDLogGPSTask, "SDLogGPSTask", 10000, NULL, 1, &SDLogGPSTaskHandle, app_cpu);

    xTaskCreatePinnedToCore(FlightControlTask, "FlightControlTask", 10000, NULL, 1, &FlightControlTaskHandle, tskNO_AFFINITY);

    // Delete "setup and loop" task

    vTaskDelete(NULL);
}

void loop() {}
