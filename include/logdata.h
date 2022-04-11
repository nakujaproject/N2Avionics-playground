#ifndef LOGDATA_H
#define LOGDATA_H

#include <mySD.h>
#include <string>
#include "defs.h"
#define LOG_FILE "flightData.txt"

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
void readSD()
{
    dataFile = SD.open("flight.txt");
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
void startWriting()
{
    dataFile = SD.open("flight.txt", FILE_WRITE);
    if (dataFile)
    {
        Serial.println("Start writing to test2");
        dataFile.println("Bange is Boss");
        dataFile.close();
    }
    else
    {

        Serial.println("Error Opening file");
    }
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(const char *message)
{
    dataFile = SD.open("flight.txt", FILE_WRITE);
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

// Write the sensor readings on the SD card
void logSDCard(LogData ld)
{
    // String dataMessage;
    // dataMessage = String(ld.counter) + "," + String(ld.altitude) + "," + String(ld.ax) + "," + String(ld.ay) + "," + String(ld.az) + "," + String(ld.gx) + "," + String(ld.gy) + "," + String(ld.gz) + "," + String(ld.filtered_s) + "," + String(ld.filtered_v) + "," + String(ld.filtered_a) + "," + String(ld.state) + "," + String(ld.longitude) + "," + String(ld.latitude) + ",";
    // appendFile(dataMessage.c_str());
}

#endif
