#ifndef READSENSORS_H
#define READSENSORS_H

// #include <FS.h>
// #include <SD.h>
// #include <SPI.h>
#include <mySD.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "logdata.h"
#include "defs.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus mygps;
// using uart 2 for serial communication
HardwareSerial neogps(2);

Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;

// function to initialize sensors and the sd card module
void init_components()
{

    Serial.println("BMP180 test!");

    if (!bmp.begin())
    {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1)
        {
            delay(SHORT_DELAY);
        }
    }
    Serial.println("BMP180 Found!");

    Serial.println("MPU6050 test!");
    if (!mpu.begin())
    {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        while (1)
        {
            delay(SHORT_DELAY);
        }
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    Serial.print("\nInitializing SD card...");

    if (!SD.begin(SDCARD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN))
    {
        Serial.println("initialization failed.");
        while (1)
        {
            delay(SHORT_DELAY);
        }
    }
    else
    {
        Serial.println("Wiring is correct and a card is present.");
    }
    Serial.println("initialization done.");

    

}

void printGPSSerial()
{
    bool newData = false;
    while (neogps.available() > 0)
    {
        if (mygps.encode(neogps.read()))
        {
            newData = true;
        }
    }
    if (newData == true)
    {
        if (mygps.location.isValid())
        {
            Serial.println(mygps.location.lat());
            Serial.println(mygps.location.lng());
            Serial.println(mygps.speed.kmph());
            Serial.println(mygps.satellites.value());
            Serial.println(mygps.altitude.meters());
        }
        else
        {
            Serial.println("Invalid location");
        }
        if (mygps.date.isValid())
        {
            Serial.print(mygps.date.month());
            Serial.print(F("/"));
            Serial.print(mygps.date.day());
            Serial.print(F("/"));
            Serial.print(mygps.date.year());
        }
        else
        {

            Serial.println("Invalid date");
        }
        if (mygps.time.isValid())
        {
            if (mygps.time.hour() < 10)
                Serial.print(F("0"));
            Serial.print(mygps.time.hour());
            Serial.print(F(":"));
            if (mygps.time.minute() < 10)
                Serial.print(F("0"));
            Serial.print(mygps.time.minute());
            Serial.print(F(":"));
            if (mygps.time.second() < 10)
                Serial.print(F("0"));
            Serial.print(mygps.time.second());
            Serial.print(F("."));
            if (mygps.time.centisecond() < 10)
                Serial.print(F("0"));
            Serial.print(mygps.time.centisecond());
        }
        else
        {
            Serial.print(F("invalid time"));
        }
    }
    else
    {
        if (millis() > 5000 && mygps.charsProcessed() < 10)
        {
            Serial.println(F("No gps detected: check wiring."));
            while (true)
                ;
        }
    }
}

struct GPSReadings get_gps_readings()
{
    struct GPSReadings gpsReadings;
    bool newData = false;
    while (neogps.available() > 0)
    {
        if (mygps.encode(neogps.read()))
        {
            newData = true;
        }
    }
    if (newData == true)
    {
        if (mygps.location.isValid())
        {
            gpsReadings.latitude = mygps.location.lat();
            gpsReadings.longitude = mygps.location.lng();
            gpsReadings.speed = mygps.speed.kmph();
            gpsReadings.satellites = mygps.satellites.value();
            gpsReadings.altitude = mygps.altitude.meters();
        }
    }
    return gpsReadings;
}

struct SensorReadings get_readings()
{
    struct SensorReadings return_val;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return_val.altitude = bmp.readAltitude(SEA_LEVEL_PRESSURE);
    return_val.ax = a.acceleration.x + 0.35;
    return_val.ay = a.acceleration.y - 0.3;
    return_val.az = a.acceleration.z - 10.31;

    // TODO: calibrate gyro
    return_val.gx = g.gyro.x;
    return_val.gy = g.gyro.y;
    return_val.gz = g.gyro.z;

    return return_val;
}

#endif