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

SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;

Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;

// function to initialize sensors and the sd card module
void init_components()
{
    Serial.begin(BAUD_RATE);
    Serial.println("BMP180 test!");
    ss.begin(115200);
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

    // Serial.print("\nInitializing SD card...");
    //  pinMode(SDCARD_CS_PIN, OUTPUT);

    // if (!SD.begin(SDCARD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN))
    // {
    //     Serial.println("initialization failed.");
    //     while (1)
    //     {
    //         delay(SHORT_DELAY);
    //     }
    // }
    // else
    // {
    //     Serial.println("Wiring is correct and a card is present.");
    // }
    // Serial.println("initialization done.");

    // startWriting();

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}
struct GPSReadings get_gps_readings()
{
    struct GPSReadings gpsReadings;
    while (ss.available() > 0)
    {
        gps.encode(ss.read());
        if (gps.location.isUpdated())
        {
            gpsReadings.latitude = gps.location.lat();
            gpsReadings.longitude = gps.location.lng();
        }
        else
        {
            gpsReadings.latitude =0;
            gpsReadings.longitude = 0;
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