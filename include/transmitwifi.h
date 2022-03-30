#ifndef TRANSMITWIFI_H
#define TRANSMITWIFI_H

#include <WiFiUdp.h>
#include "defs.h"
#include "kalmanfilter.h"

#include <WebServer.h>
#include <ArduinoJson.h>

WiFiUDP Udp;
WebServer server(80);

void serveDataUDP(LogData ld)
{
  Udp.beginPacket({192, 168, 4, 255}, UDP_PORT);

  // payload

  Udp.printf("Counter : %d \n Altitude : %.3f \n ax : %.3f \n ay : %.3f \n az  : %.3f \n gx  : %.3f \n gy  : %.3f \n gz  : %.3f \n s : %.3f \n v : %.3f \n a : %.3f \n Current State  : %d \n Longitude  : %.3f \n Latitude  : %.3f \n", ld.counter, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.state, ld.longitude, ld.latitude);

  if (!Udp.endPacket())
  {
    Serial.println("NOT SENT!");
  }
  else
  {
    Serial.println("SENT!!");
  }
}

void serve_data()
{
  LogData ld = readData();
  DynamicJsonDocument doc(256);

  doc["counter"] = ld.counter;
  doc["altitude"] = ld.altitude;
  doc["ax"] = ld.ax;
  doc["ay"] = ld.ay;
  doc["az"] = ld.az;
  doc["gx"] = ld.gx;
  doc["gy"] = ld.gy;
  doc["gz"] = ld.gz;
  doc["filtered_s"] = ld.filtered_s;
  doc["filtered_v"] = ld.filtered_v;
  doc["filtered_a"] = ld.filtered_a;
  doc["state"] = ld.state;
  doc["longitude"] = ld.longitude;void serve_data()
{
  LogData ld = readData();
  DynamicJsonDocument doc(256);

  doc["counter"] = ld.counter;
  doc["altitude"] = ld.altitude;
  doc["ax"] = ld.ax;
  doc["ay"] = ld.ay;
  doc["az"] = ld.az;
  doc["gx"] = ld.gx;
  doc["gy"] = ld.gy;
  doc["gz"] = ld.gz;
  doc["filtered_s"] = ld.filtered_s;
  doc["filtered_v"] = ld.filtered_v;
  doc["filtered_a"] = ld.filtered_a;
  doc["state"] = ld.state;
  doc["longitude"] = ld.longitude;
  doc["latitude"] = ld.latitude;

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}
  doc["latitude"] = ld.latitude;

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}
void handle_on_connect()
{
  server.send(200, "text/plain", "Hello client");
}

void handle_not_found()
{
  server.send(404, "text/plain", "Not Found");
}

void setupServer()
{

  // handle HTTP REQUESTS
  server.on("/", handle_on_connect);

  server.on("/data", serve_data);

  server.onNotFound(handle_not_found);

  // start server
  server.begin();
  Serial.println("\nHTTP Server started...");
}

#endif