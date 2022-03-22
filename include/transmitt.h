#ifndef TRANSMITT_H
#define TRANSMITT_H

#include <WiFiUdp.h>
#include "defs.h"

WiFiUDP Udp;

void serveData(LogData ld)
{
  Udp.beginPacket({192, 168, 4, 255}, UDP_PORT);

  // payload

  Udp.printf("Counter : %f \n Altitude : %.3f \n ax : %.3f \n ay : %.3f \n az  : %.3f \n gx  : %.3f \n gy  : %.3f \n gz  : %.3f \n s : %.3f \n v : %.3f \n a : %.3f \n Current State  : %d \n Longitude  : %.3f \n Latitude  : %.3f \n", ld.counter, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.states, ld.longitude, ld.latitude);

  if (!Udp.endPacket())
  {
    Serial.println("NOT SENT!");
  }
  else
  {
    Serial.println("SENT!!");
  }
}

#endif