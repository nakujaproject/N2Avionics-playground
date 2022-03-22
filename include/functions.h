#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include "variables.h" // header file containing variables
#include "defs.h" // header file containing constants

void createAccessPoint(){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting Ground Station WiFi...");
  delay(1000);

  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, key);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("ssid:"); Serial.print(ssid); Serial.println();
  Serial.print("key:"); Serial.print(key); Serial.println();
  Serial.print("Rocket IP address: "); Serial.print(IP); Serial.println();
  
  server.begin();
}

#endif