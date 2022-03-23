
#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */
#include "../include/functions.h"
#include "../include/readsensors.h"
#include "../include/kalmanfilter.h"
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/transmitt.h"

// TaskHandle_t Task1;
// TaskHandle_t Task2;

// void Task1Code(void* pvParameters){
//   for(;;){

//       // read sensors
//     get_readings();

//     // send data to ground station
//     // serveData();

//     // print Sensor values for debugging
//     printToSerial();

//     // Write sensor reading to SD card
//     //logToSD();
//   }

// }

// void Task2Code(void* pvParameters){
//   for(;;){
//     // kalmanUpdate();
//     // detectLiftoff();
//     // detectApogee();
//   }
// }

int counter = 0;
int state = 0;
void setup()
{
  // init_components();
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(BAUD_RATE);
  Serial.println("Bange is boss");
  createAccessPoint();
}

void loop()
{
  // struct SensorReadings readings;
  // struct FilteredValues filtered_values;
  // readings = get_readings();
  // filtered_values = kalmanUpdate(readings.altitude, readings.az);
  // state = checkState(filtered_values.displacement, filtered_values.velocity, counter, state);
  // counter = counter + 1;

  struct LogData ld;
  ld.counter = 1;
  ld.altitude = 1.2;
  ld.ax = 1.2;
  ld.ay = 1.2;
  ld.az = 1.2;
  ld.gx = 5.2;
  ld.gy = 3.6;
  ld.gz = 69.2;
  ld.filtered_s = 1.2;
  ld.filtered_v = 1.2;
  ld.filtered_a = 1.2;
  ld.states = 3;
  ld.longitude = 2.4;
  ld.latitude = 3.2;

  serveData(ld);
}
