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

    sprintf(message, "{\"counter\":%lld,\"state\":%d,\"altitude\":%.3f}\n", sv.timeStamp, sv.state, sv.altitude);
    return message;
}

void sendTelemetryLora(SendValues sv[5])
{
    LoRa.beginPacket();

    char combinedMessage[1280];
    for (int i = 0; i < 5; i++)
    {

        char *message = printLoraMessage(sv[i]);
        sprintf(combinedMessage, message);
        vPortFree(message);
    }
    debugln(combinedMessage);
    LoRa.print(combinedMessage);
    // send packet
    LoRa.endPacket();
    debugln("Done");
}

int processInputCommand(int currentState)
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

int handleLora(int currentState, SendValues sv[5])
{

    int rval;
    // check for incoming data stream
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        rval = processInputCommand(currentState);
    }
    else
    {
        sendTelemetryLora(sv);
        rval = currentState;
    }
    return rval;
}

#endif
