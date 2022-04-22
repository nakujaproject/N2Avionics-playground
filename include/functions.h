#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defs.h" // header file containing constants
#include <WiFi.h>

const char *ssid = "appendix";
const char *key = "123456789";

void createAccessPoint()
{
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting Ground Station WiFi...");
  WiFi.mode(WIFI_AP);

  delay(1000);

  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, key);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("ssid:");
  Serial.print(ssid);
  Serial.println();
  Serial.print("key:");
  Serial.print(key);
  Serial.println();
  Serial.print("Rocket IP address: ");
  Serial.print(IP);
  Serial.println();
}

void ejection()
{
  //TODO: set timer here
  digitalWrite(EJECTION_PIN, HIGH);
}

void done_ejection()
{
  digitalWrite(EJECTION_PIN, LOW);
}

// struct LogData dummyData()
// {
//   struct LogData ld;
//   ld.counter = 2;
//   ld.altitude = 10.5;
//   ld.ax = 2.56;
//   ld.ay = 2.65;
//   ld.az = 1.02;
//   ld.gx = 5.8;
//   ld.gy = 85.6;
//   ld.gz = 2.6;
//   ld.filtered_s = 3.56;
//   ld.filtered_a = 9.85;
//   ld.filtered_v = 1005.3;
//   ld.state = 5;

//   // TODO: gps
//   ld.longitude = 2.4;
//   ld.latitude = 3.2;

//   return ld;
// }

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
  ld.latitude=gpsReadings.latitude;
  ld.longitude=gpsReadings.longitude;
  return ld;
}
#endif