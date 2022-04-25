#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defs.h" // header file containing constants

void ejection()
{
  //TODO: set timer here
  digitalWrite(EJECTION_PIN, HIGH);
}

void done_ejection()
{
  digitalWrite(EJECTION_PIN, LOW);
}

struct LogData dummyData()
{
  struct LogData ld;
  ld.counter = 2;
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

// This formats data that we are going to save to memory
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

// This formats data we are going to send
// Currently we are sending altitude, state and counter
struct SendValues formart_send_data(LogData readings)
{
  struct SendValues sv;
  sv.altitude = readings.altitude;
  sv.state = readings.state;
  sv.counter = readings.counter;
  return sv;
}

#endif