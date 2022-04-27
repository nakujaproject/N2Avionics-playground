#ifndef LOGDATA_H
#define LOGDATA_H

#include <mySD.h>
#include <string>
#include "defs.h"

File dataFile;

void startWriting()
{
    dataFile = SD.open(telemetryLogFile, FILE_WRITE);
    if (dataFile)
    {
        debug("Start writing to ");
        debugln(telemetryLogFile);
        dataFile.close();
    }
    else
    {
        debug("Error Opening ");
        debugln(telemetryLogFile);
    }
}

char *printSDMessage(LogData ld)
{
    // The assigned size is calculated to fit the string
    char *message = (char *)pvPortMalloc(sizeof(char) * 256);
    if (!message)
        return NULL;
    snprintf(message, 256, "{\"counter\":%lld,\"sensor altitude\":%.3f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,\"filtered s\":%.3f,\"filtered v\":%.3f,\"filtered a\":%.3f,\"state\":%d,\"gps altitude\":%.3f,\"longitude\":%.8f,\"latitude\":%.8f}\n", ld.timeStamp, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.state, ld.gpsAltitude, ld.longitude, ld.latitude);
    return message;
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendToFile(LogData ld[5])
{

    dataFile = SD.open(telemetryLogFile, FILE_WRITE);
    if (!dataFile)
    {
        debugln("Failed to open file for appending");
        return;
    }
    char combinedMessage[1280];
    for (int i = 0; i < 5; i++)
    {

        char *message = printSDMessage(ld[i]);
        sprintf(combinedMessage, message);
        vPortFree(message);
    }
    debugln(combinedMessage);

    if (dataFile.println(combinedMessage))
    {
        debugln("Message appended\n");
    }
    else
    {
        debugln("Append failed");
    }

    dataFile.close();
}

#endif
