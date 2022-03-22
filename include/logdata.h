#ifndef LOGDATA_H
#define LOGDATA_H

#include <BasicLinearAlgebra.h>

#define LOG_FILE "loggeddata.txt"

struct LogData {
    float counter;
    float altitude;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float filtered_s;
    float filtered_v;
    float filtered_a;
    float states;
    float longitude;
    float latitude;

}
void startWriting(){
    dataFile = SD.open(LOG_FILE, FILE_WRITE);
    if (dataFile)
    {
        Serial.println("Start writing to test2");
        dataFile.println("Index, Altitude, ax, ay, az, gx, gy, gz, filtered_s, filtered_v, filtered_a, states, longitude, latitude \r\n");
        dataFile.close();
    }
    else
    {
        Serial.println("File already exists");
    }
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(const char *message){
    dataFile = SD.open(LOG_FILE, FILE_WRITE);
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
void logSDCard(LogData ld) {
    String dataMessage
    dataMessage = String(ld.counter) + "," + String(ld.altitude) + "," + String(ld.ax) + "," + String(ld.ay) + "," + String(ld.az) + "," + String(ld.gx) + "," + String(ld.gy) + "," + String(ld.gz) + "," + String(ld.filtered_s) + "," + String(ld.filtered_v) + "," + String(ld.filtered_a) + "," + String(ld.states) + "," + String(ld.longitude) + "," + String(ld.latitude) ",";
    appendFile(dataMessage.c_str());
}

#endif
