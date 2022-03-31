#ifndef TRANSMITLORA_H
#define TRANSMITLORA_H

#include <SPI.h>
#include <LoRa.h>
#include "defs.h"

const long freq = 868E6; // frequechy 868 MHz
const int SF = 9;        // spread factor
const long bw = 125E3;   // bandwidth 125 kHz

void setUpLoraOnBoard()
{

    LoRa.setPins(CS_LORA_PIN, RESET_LORA_PIN, IRQ_LORA_PIN); // set CS, reset, IRQ pin
    Serial.print("Setting up LoRa Sender...");

    while (!LoRa.begin(freq))
    {
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Successfully set up LoRa");

    LoRa.setSpreadingFactor(SF);
    // LoRa.setSignalBandwidth(bw);
    LoRa.setSyncWord(0xF3);
    Serial.print("Frequency ");
    Serial.print(freq);
    Serial.print(" Bandwidth ");
    Serial.print(bw);
    Serial.print(" SF ");
    Serial.println(SF);
}

void sendMessageLora(LogData ld)
{
    char message[256];
    sprintf(message, "{\"Counter\":%d,\"Altitude\":%.3f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,\"s\":%.3f,\"v\":%.3f,\"a\":%.3f,\"Current State\":%d,\"Longitude\":%.3f,\"Latitude\":%.3f\n}", ld.counter, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.state, ld.longitude, ld.latitude);
    Serial.printf("Message %d\n", ld.counter);
    Serial.println(message);
    // send packet
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
}
#endif