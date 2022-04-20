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
SoftwareSerial GPSModule(GPS_RX_PIN, GPS_TX_PIN); // RX, TX

Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;

int updates;
int failedUpdates;
int pos;
int stringplace = 0;

String timeUp;
String nmea[15];
String labels[12]{"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

String ConvertLat()
{
    String posneg = "";
    if (nmea[3] == "S")
    {
        posneg = "-";
    }
    String latfirst;
    float latsecond;
    for (int i = 0; i < nmea[2].length(); i++)
    {
        if (nmea[2].substring(i, i + 1) == ".")
        {
            latfirst = nmea[2].substring(0, i - 2);
            latsecond = nmea[2].substring(i - 2).toFloat();
        }
    }
    latsecond = latsecond / 60;
    String CalcLat = "";

    char charVal[9];
    dtostrf(latsecond, 4, 6, charVal);
    for (int i = 0; i < sizeof(charVal); i++)
    {
        CalcLat += charVal[i];
    }
    latfirst += CalcLat.substring(1);
    latfirst = posneg += latfirst;
    return latfirst;
}

String ConvertLng()
{
    String posneg = "";
    if (nmea[5] == "W")
    {
        posneg = "-";
    }

    String lngfirst;
    float lngsecond;
    for (int i = 0; i < nmea[4].length(); i++)
    {
        if (nmea[4].substring(i, i + 1) == ".")
        {
            lngfirst = nmea[4].substring(0, i - 2);
            // Serial.println(lngfirst);
            lngsecond = nmea[4].substring(i - 2).toFloat();
            // Serial.println(lngsecond);
        }
    }
    lngsecond = lngsecond / 60;
    String CalcLng = "";
    char charVal[9];
    dtostrf(lngsecond, 4, 6, charVal);
    for (int i = 0; i < sizeof(charVal); i++)
    {
        CalcLng += charVal[i];
    }
    lngfirst += CalcLng.substring(1);
    lngfirst = posneg += lngfirst;
    return lngfirst;
}

// function to initialize sensors and the sd card module
void init_components()
{

    // GPS Software Serial
    // GPSModule.begin(9600);

    // Serial.println("BMP180 test!");

    // if (!bmp.begin())
    // {
    //     Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    //     while (1)
    //     {
    //         delay(SHORT_DELAY);
    //     }
    // }
    // Serial.println("BMP180 Found!");

    // Serial.println("MPU6050 test!");
    // if (!mpu.begin())
    // {
    //     Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    //     while (1)
    //     {
    //         delay(SHORT_DELAY);
    //     }
    // }
    // Serial.println("MPU6050 Found!");
    // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    // Serial.print("\nInitializing SD card...");

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
    
    // startWriting(telemetryLogFile);
    // startWriting(gpsLogFile);
}

struct GPSReadings get_gps_readings()
{
    struct GPSReadings gpsReadings;
    Serial.flush();
    GPSModule.flush();
    while (GPSModule.available() > 0)
    {
        GPSModule.read();
    }
    if (GPSModule.find("$GPRMC,"))
    {
        String tempMsg = GPSModule.readStringUntil('\n');
        for (int i = 0; i < tempMsg.length(); i++)
        {
            if (tempMsg.substring(i, i + 1) == ",")
            {
                nmea[pos] = tempMsg.substring(stringplace, i);
                stringplace = i + 1;
                pos++;
            }
            if (i == tempMsg.length() - 1)
            {
                nmea[pos] = tempMsg.substring(stringplace, i);
            }
        }
        updates++;
        //    nmea[2] = ConvertLat();
        //    nmea[4] = ConvertLng();
        float lati = ConvertLat().toFloat();
        float lngi = ConvertLng().toFloat();
        gpsReadings.latitude=lati;
        gpsReadings.longitude=lngi;
    }
    else
    {

        failedUpdates++;
    }
    stringplace = 0;
    pos = 0;

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

    return_val.gx = g.gyro.x;
    return_val.gy = g.gyro.y;
    return_val.gz = g.gyro.z;

    return return_val;
}

#endif