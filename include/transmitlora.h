#ifndef TRANSMITLORA_H
#define TRANSMITLORA_H

#include <SPI.h>
#include <LoRa.h>
#include "defs.h"
#include "readsensors.h"

char *printLoraMessage(SendValues sv)
{
    // The assigned size is calculated to fit the string
    char *message = (char *)pvPortMalloc(48);

    if (!message)
        return NULL;

    sprintf(message, "{\"counter\":%d,\"state\":%d,\"altitude\":%.3f}\n", sv.counter, sv.state, sv.altitude);
    return message;
}

void sendTelemetryLora(SendValues sv[5])
{
    LoRa.beginPacket();
    for (int i = 0; i < 4; i++)
    {

        char *message = printLoraMessage(sv[i]);
        debugln(message);
        LoRa.print(message);
        vPortFree(message);
    }
    // send packet

    LoRa.endPacket();
    debugln("Done");
}

int processInputCommand(int currentState)
{

    // check for incoming data stream
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packet
        if (LoRa.available() > 0)
        {
            int command = LoRa.parseInt();
            switch (command)
            {
            case 0:
                return 0;
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            case 4:
                return 4;
            default:
                return 0;
            }
        }
        else
        {
            return currentState;
        }
    }
    else
    {
        return currentState;
    }
}

#endif
