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

#define DEBUG 1
#if DEBUG ==1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

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
    struct SensorReadings readings;
    struct FilteredValues filtered_values;
    struct LogData ld;
    struct GPSReadings gpsReadings;

    readings = get_readings();

    // TODO: very important to know the orientation of the altimeter
    filtered_values = kalmanUpdate(readings.altitude, readings.ay);

    //using mutex since we are modifying a volatile var
    
    portENTER_CRITICAL_ISR(&mutex);
    // TODO: why is the counter being passed here?
    state = checkState(filtered_values.displacement, filtered_values.velocity, 0, state);
    portEXIT_CRITICAL_ISR(&mutex);
    
    
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

        // Ticks to wait has been optimized
        if (xQueueReceive(telemetry_queue, (void *)&ld, 10) == pdTRUE)
        {
            sendTelemetryLora(ld);
        }

        // yield to another task
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void LoRaReceiveCommandTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {

        // check for incoming data stream
        int packetSize = LoRa.parsePacket();
        if (packetSize)
        {
            // received a packet

            if (LoRa.available() > 0)
            {
                int command = LoRa.parseInt();

                portENTER_CRITICAL_ISR(&mutex);
                switch (command)
                {
                case 0:
                    state = 0;
                    break;
                case 1:
                    state = 1;
                    break;
                case 2:
                    state = 2;
                    break;
                case 3:
                    state = 3;
                    break;
                case 4:
                    state = 4;
                    break;
                case 5:
                    state = 5;
                    break;
                case 6:
                    ejection();
                    break;
                default:
                    break;
                }
                portEXIT_CRITICAL_ISR(&mutex);
            }
        }

        // yield to another task
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void ReadTelemetryTask(void *parameter)
{

    struct LogData ld;
    for (;;)
    {

        ld = readData();
        if (xQueueSend(telemetry_queue, (void *)&ld, 0) != pdTRUE)
        {
            debug("Queue Full!");
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
            appendToFile(ld);
            free(message);
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

    init_components(spi);

    // get the base_altitude
    SensorReadings readings = get_readings();
    FilteredValues filtered_values = kalmanUpdate(readings.altitude, readings.ay);
    base_altitude = filtered_values.displacement;

    delay(SETUP_DELAY)

    telemetry_queue = xQueueCreate(telemetry_queue_length, sizeof(LogData));

    // initialize core tasks
    // TODO: optimize the stackdepth
    xTaskCreatePinnedToCore(LoRaTelemetryTask, "LoRaTelemetryTask", 250, NULL, 1, &LoRaTelemetryTaskHandle, pro_cpu);

    xTaskCreatePinnedToCore(ReadTelemetryTask, "ReadTelemetryTask", 250, NULL, 1, &ReadTelemetryTaskHandle, app_cpu);

    xTaskCreatePinnedToCore(SDLogTelemetryTask, "SDLogTelemetryTask", 250, NULL, 1, &SDLogTelemetryTaskHandle, app_cpu);

    xTaskCreatePinnedToCore(LoRaReceiveCommandTask, "LoRaReceiveCommandTask", 250, NULL, 1, &LoRaReceiveCommandTaskHandle, pro_cpu);

    // xTaskCreatePinnedToCore(FlightControlTask, "FlightControlTask", 250, NULL, 1, &FlightControlTaskHandle, tskNO_AFFINITY);

    // Delete "setup and loop" task
    vTaskDelete(NULL);
}
