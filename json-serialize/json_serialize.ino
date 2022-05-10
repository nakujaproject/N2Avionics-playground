#include <ArduinoJson.h>

// WIFI libraries
#include <WiFi.h>
#include <WebServer.h>

// MPU6050 Gyroscope libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// BMP180 libraries
#include <Adafruit_BMP085.h>

Adafruit_MPU6050 mpu;

// function declarations
ArduinoJson6191_F1::DynamicJsonDocument createJSON();
void readMPU();

float altitude, ax, ay, az;
String data_message;


void init_MPU(){
  
  // initialize mpu6050
  if(!mpu.begin()){
    Serial.println("MPU6050 Not found..."); 
    while(1){
      delay(10); 
    }
  }  
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

}

void readMPU(){

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    ax = a.acceleration.x;
    ay = a.acceleration.y;
    az = a.acceleration.z;
}

ArduinoJson6191_F1::DynamicJsonDocument createJSON(){ // fucntion datatype based on ArduinoJson library
  readMPU();
  const int capacity = JSON_OBJECT_SIZE(4); // one array with 4 elements
  
  // create json document
  DynamicJsonDocument doc(capacity);
  
  return doc;
   
}

void setup() {
  Serial.begin(115200);
  init_MPU();
  
  Serial.println();
}

void loop() { 
  // create a JSON document
  ArduinoJson6191_F1::DynamicJsonDocument file = createJSON();
  
  // read mpu values
  readMPU();

  // construct the data string message
  data_message = String(altitude) + "," + String(ax) + "," + String(ay) + "," + String(az);

  Serial.println(data_message);
//  file["data"] = data_message;
    
//  serializeJson(file, Serial); // print array data to Serial monitor
}
