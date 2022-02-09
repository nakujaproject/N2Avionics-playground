/*
 * This code measures the strength of the  value of the WIFI
 *
 */

#include <WiFi.h>


char ssid[] = "iPIC-WIRELESS";
char pass[] = "987654321jica";

void setup() {
  WiFi.begin(ssid, pass);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    while(true);
  }
  
  // if you are connected, print out info about the connection:
  else {
     // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);
  }

}

void loop() {
  
}
