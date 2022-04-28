#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defs.h" // header file containing constants
#include "readsensors.h"

// Ejection Fires the explosive charge using a relay or a mosfet
void Ejection()
{
  // TODO: set timer here
  digitalWrite(EJECTION_PIN, HIGH);
}

// Done_ejection turns of the pin that fired the ejection charge
void Done_ejection()
{
  digitalWrite(EJECTION_PIN, LOW);
}

// DummyData returns a dummy data sample for LogData
// This is helpful for analysis
struct LogData DummyData()
{
  struct LogData ld;
  ld.timeStamp = 2;
  ld.altitude = 10.5;
  ld.ax = 2.56;
  ld.ay = 2.65;
  ld.az = 1.02;
  ld.gx = 5.8;
  ld.gy = 85.6;
  ld.gz = 2.6;
  ld.filtered_s = 3.56;
  ld.filtered_a = 9.85;
  ld.filtered_v = 1005.3;
  ld.state = 5;
  ld.longitude = 2.4;
  ld.latitude = 3.2;
  return ld;
}

// formart_data This formats data that we are going to save to SD card
// We pass in our SensorReadings, FilteredValues and GPSReadings
// We save all the data points we are collecting
struct LogData formart_data(SensorReadings readings, FilteredValues filtered_values, GPSReadings gpsReadings)
{
  struct LogData ld;
  ld.altitude = readings.altitude;
  ld.ax = readings.ax;
  ld.ay = readings.ay;
  ld.az = readings.az;
  ld.gx = readings.gx;
  ld.gy = readings.gy;
  ld.gz = readings.gz;
  ld.filtered_s = filtered_values.displacement;
  ld.filtered_a = filtered_values.acceleration;
  ld.filtered_v = filtered_values.velocity;
  ld.latitude = gpsReadings.latitude;
  ld.longitude = gpsReadings.longitude;
  return ld;
}

// formart_send_data This formats data we are going to send over LoRa
// Currently we are sending altitude, state and timeStamp
struct SendValues formart_send_data(LogData readings)
{
  struct SendValues sv;
  sv.altitude = readings.altitude;
  sv.state = readings.state;
  sv.timeStamp = readings.timeStamp;
  return sv;
}

// get_base_altitude Finds the average of the current altitude from 1000 readings
float get_base_altitude()
{
  float altitude = 0;
  SensorReadings readings;
  for (int i = 0; i < 1000; i++)
  {
    readings = get_readings();
    altitude = altitude + readings.altitude;
    delay(SHORT_DELAY);
  }
  return altitude / 1000.0;
}
#endif