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

static const BaseType_t pro_cpu = 0;

static const BaseType_t app_cpu = 1;

TaskHandle_t LoRaTelemetryTaskHandle;

TaskHandle_t ReadTelemetryTaskHandle;

TaskHandle_t SDLogTelemetryTaskHandle;

TaskHandle_t FlightControlTaskHandle;

TaskHandle_t LoRaReceiveCommandTaskHandle;

static uint8_t telemetry_queue_length = 10;
static QueueHandle_t telemetry_queue;

portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

volatile int state;

// uninitalised pointers to SPI objects
SPIClass *hspi = NULL;

struct LogData readData()
{
    struct LogData ld;
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct GPSReadings gpsReadings;

    readings = get_readings();

    // TODO: very important to know the orientation of the altimeter
    filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    // using mutex since we are modifying a volatile var

    portENTER_CRITICAL_ISR(&mutex);
    state = checkState(filtered_values.displacement, filtered_values.velocity, state);
    portEXIT_CRITICAL_ISR(&mutex);

    // if we reach ground state we can start reading gpsData
    if (state == 4)
    {
        gpsReadings = get_gps_readings();
    }

    ld = formart_data(readings, filtered_values, gpsReadings);
    ld.state = state;

    return ld;
}

void GetDataTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {

        ld = readData();
        if (xQueueSend(telemetry_queue, (void *)&ld, 0) != pdTRUE)
        {
            debug("Queue Full!");
        }
    }
}

void LoRaTelemetryTask(void *parameter)
{

    struct LogData ld;
    struct SendValues sv;
    struct SendValues svRecords[5];

    for (;;)
    {
        for (int i = 0; i < 4; i++)
        {
            xQueueReceive(telemetry_queue, (void *)&ld, 10);
            sv = formart_send_data(ld);
            svRecords[i] = sv;
        }
        sendTelemetryLora(svRecords);
        // yield to another task
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void LoRaReceiveCommandTask(void *parameter)
{

    for (;;)
    {
        portENTER_CRITICAL_ISR(&mutex);
        state = processInputCommand(state);
        portEXIT_CRITICAL_ISR(&mutex);

        // yield to another task
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
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

    init_components(spi);

    // get the base_altitude
    SensorReadings readings = get_readings();

    FilteredValues filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    BASE_ALTITUDE = filtered_values.displacement;

    delay(SETUP_DELAY);

    telemetry_queue = xQueueCreate(telemetry_queue_length, sizeof(LogData));

    // initialize core tasks
    // TODO: optimize the stackdepth
    xTaskCreatePinnedToCore(LoRaTelemetryTask, "LoRaTelemetryTask", 250, NULL, 1, &LoRaTelemetryTaskHandle, pro_cpu);
    xTaskCreatePinnedToCore(LoRaTelemetryTask, "LoRaTelemetryTask", 250, NULL, 1, &LoRaTelemetryTaskHandle, pro_cpu);
    xTaskCreatePinnedToCore(GetDataTask, "GetDataTask", 250, NULL, 1, &ReadTelemetryTaskHandle, app_cpu);

    xTaskCreatePinnedToCore(LoRaReceiveCommandTask, "LoRaReceiveCommandTask", 250, NULL, 2, &LoRaReceiveCommandTaskHandle, pro_cpu);
    // Delete "setup and loop" task
    vTaskDelete(NULL);
}
