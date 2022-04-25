#ifndef TRANSMITLORA_H
#define TRANSMITLORA_H

#include <SPI.h>
#include <LoRa.h>
#include "defs.h"
#include "readsensors.h"



char *printLoraMessage(SendValues sv)
{
    char *message = (char *)malloc(48);
    if (!message)
        return NULL;
    
    sprintf(message, "{\"counter\":%d,\"state\":%d,\"altitude\":%.3f}", sv.counter, sv.state, sv.altitude);
    return message;
}

void sendTelemetryLora(SendValues sv)
{

    char *message = printLoraMessage(sv);
    debugln(message);
    // send packet
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    debugln("Done");
    free(message);
}

#endif
