#ifndef LOGDATA_H
#define LOGDATA_H

#include <mySD.h>
#include <string>
#include "defs.h"

File dataFile;
/*
* ==================== Write flight data to SD card =====================
MICROSD MODULE PINOUT
3V3 -> 3v3
CS -> GPIO5
MOSI -> GPIO23
CLK -> GPIO18
MISO -> GPIO19
GND -> GND
*/
void readSD(const char *fileName)
{
    dataFile = SD.open(fileName);
    if (dataFile)
    {
        Serial.println("Content:");

        // read from the file until there's nothing else in it:
        while (dataFile.available())
        {
            Serial.write(dataFile.read());
        }
        // close the file:
        dataFile.close();
    }
    else
    {

        Serial.println("error opening test.txt");
    }
}
void startWriting(const char *fileName)
{
    dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile)
    {
        Serial.print("Start writing to ");
        Serial.println(fileName);
        dataFile.close();
    }
    else
    {

        Serial.println("Error Opening ");
        Serial.println(fileName);
    }
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendToFile(const char *message, const char *fileName)
{
    dataFile = SD.open(fileName, FILE_WRITE);
    if (!dataFile)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (dataFile.println(message))
    {
        Serial.println("Message appended\n");
    }
    else
    {
        Serial.println("Append failed");
    }
    dataFile.close();
}

#endif
