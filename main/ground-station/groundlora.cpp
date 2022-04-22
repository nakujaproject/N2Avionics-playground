#include <Arduino.h>
#include <LoRa.h>

#include <ArduinoJson.h>

static const BaseType_t pro_cpu = 0;

static const BaseType_t app_cpu = 1;

TaskHandle_t LoRaTelemetryReceiveTaskHandle;
TaskHandle_t LoRaCommandSendTaskHandle;

const long freq = 868E6;
const int SF = 9;
const long bw = 125E3;

void setUpLoraOnBoard()
{
    LoRa.setPins(5, 25, 2); // set CS, reset, IRQ pin
    Serial.println("Setting up LoRa Receiver");

    if (!LoRa.begin(freq))
    {
        Serial.print("Failed to begin");
    }

    Serial.println();
    Serial.println("Successfully set up LoRa");

    LoRa.setSpreadingFactor(7);
    // LoRa.setSignalBandwidth(bw);
    LoRa.setSyncWord(0xF3);
    Serial.print("Frequency ");
    Serial.print(freq);
    Serial.print(" Bandwidth ");
    Serial.print(bw);
    Serial.print(" SF ");
    Serial.println(SF);
}

void LoRaTelemetryReceiveTask(void *parameter)
{
    for (;;)
    {

        // try to parse packet
        int packetSize = LoRa.parsePacket();
        if (packetSize)
        {
            // received a packet
            String message = "";
            while (LoRa.available())
            {
                message = LoRa.readString();
            }

            Serial.println(message);
        }
    }
}
void LoRaCommandSendTask(void *parameter)
{
    for (;;)
    {
        if (Serial.available() > 0)
        {
            // 0 to 5 for states
            // 6 to deploy chute
            char message = Serial.read();
            LoRa.beginPacket();
            LoRa.print(message);
            LoRa.endPacket();
            Serial.printf("Sent interrupt command %c \n", message);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setUpLoraOnBoard();
    xTaskCreatePinnedToCore(LoRaTelemetryReceiveTask, "LoRaTelemetryReceiveTask", 250, NULL, 1, &LoRaTelemetryReceiveTaskHandle, pro_cpu);
    xTaskCreatePinnedToCore(LoRaCommandSendTask, "LoRaCommandSendTask", 250, NULL, 1, &LoRaCommandSendTaskHandle, app_cpu);

    // Delete "setup and loop" task
    vTaskDelete(NULL);
}
void loop()
{
}