#ifndef TRANSMITLORA_H
#define TRANSMITLORA_H

#include <SPI.h>
#include <LoRa.h>

const long freq = 868E6; // frequechy 868 MHz
const int SF = 9;        // spread factor
const long bw = 125E3;   // bandwidth 125 kHz

void setUpLoraOnBoard()
{
    LoRa.setPins(5, 14, 2); // set CS, reset, IRQ pin
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

void sendMessageLora(String message, int counter)
{
    Serial.printf("Message %d\n", counter);
    Serial.println(message);
    // send packet
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
}
#endif