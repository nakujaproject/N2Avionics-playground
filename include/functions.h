#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BasicLinearAlgebra.h>
#include <WiFi.h>
#include <SD.h>
#include<CircularBuffer.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

#include "global_variables.h" // header file containing variables
#include "defs.h" // header file containing constants

Adafruit_BMP085 barometer; 
Adafruit_MPU6050 accelerometer;


using namespace BLA;
/* 
* ==================== FUNCTION DECLARATION ==================== 
*/
void initializeComponents();
String getSensorReadings();
void printToSerial();
void connectToWifi();
void logToSD();
void detectLiftoff();
void detectApogee();
void deployParachute();
void createAcessPoint();
// void serveData();
String request_server_data(const char* server_name);


/*
 * ==================== FUNCTION DEFINITIONS ====================
 */
void initializeComponents(){
  /*
   * This function checks and initializes all the sensors and components 
   * Params: none
   * Return: void
   */

  Serial.begin(BAUD_RATE);

 
  // Initialize BMP pressure sensor
  Serial.println("Barometer Check...");
  if(!barometer.begin()){
    Serial.println("Barometer not found...");
    while (1){
      delay(SHORT_DELAY);
    }
  }
  Serial.println("Barometer OK..."); // todo : these checks to be logged and relayed to ground as logs

  // Initialize MPU6050 accelerometer
  Serial.println("Accelerometer Check...");
  if(!accelerometer.begin()){
    Serial.println("Accelerometer not found...");
    while (1) {
      delay(SHORT_DELAY);
    }
  }
  
  Serial.println("Accelerometer OK...");
}
 
String getSensorReadings(){
  sensors_event_t a, g, temp;
  accelerometer.getEvent(&a, &g, &temp);

  // read altitude
  altitude = barometer.readAltitude(SEA_LEVEL_PRESSURE_HPA * 100);

  // convert altitude to unsigned integer
  // altitude = (int)altitude;

  // read x axis acceleration
  ax = a.acceleration.x;

  // read y axis acceleration
  ay = a.acceleration.y;

  // read z axis acceleration
  az = a.acceleration.z;

  // append data to sata_message
  data_message = String("Altitude: ") + String(altitude) + ", " + String("Acc-x: ") + String(ax) + ", " + String("Acc-y: ") + String(ay) + ", " + String("Acc-z: ") + String(az) + "\n";
  return data_message;
}

void printToSerial(){
  //Data = String(altitude) + "," + String(ax) + "," + String(ay) + "," + String(az);
  Serial.print("Altitude:"); Serial.print(altitude); Serial.println();
  Serial.print("ax: "); Serial.print(ax); Serial.println();
  Serial.print("ay: "); Serial.print(ay); Serial.println();
  Serial.print("az: "); Serial.print(az); Serial.println();
  Serial.println();

  delay(100);
}

// void connectToWifi(){
//   WiFi.begin(ssid, key);

//   // check if connection okay
//   while(WiFi.status() != WL_CONNECTED){
//     Serial.println("Establishing connection to WiFi...");
//     delay(SHORT_DELAY);
//   }

//   // connection okay
//   Serial.print("Connection established to "); Serial.print(ssid);
//   Serial.println(WiFi.localIP()); 
  
// }

void logToSD(){
	/*
	* ==================== Write flight data to SD card =====================
	*/


}

void detectLiftoff(){
  /*
	* ==================== Detect when the rocket has launched =====================
  * If the current altitude is greater than the previous by 50cm
  * report lift-off
	*/
  current_altitude = barometer.readAltitude(SEA_LEVEL_PRESSURE_HPA * 100);

  if(is_lift_off == false){
     if((current_altitude - CURRENT_ALTITUDE) > LIFT_OFF_DEVIATION){   
        Serial.println("(0) Waiting for lift-off...");

        Serial.println(current_altitude);

        // lift off detected
        Serial.print("Lift Off!"); // log this to some file
        is_lift_off = true;
    }
  }
}

void detectApogee(){
  /*
  * Detect point where velocity is at 0
  * v = u + at
  */
  
  Serial.println("(1) Waiting for apogee...");

  // circular buffer 


}

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

// void serveData(){

//   unsigned long currentMillis = millis();
//   if(currentMillis - previousMillis >= interval){
//     if(WiFi.status() != WL_CONNECTED){
//       // wifi not connected
//       Serial.println("Wifi disconnected. Please try reconnecting...");

//       Serial.println("DATA: " + sensor_data);  //change this to send all required data at once
//     }
//   }

// }



#endif