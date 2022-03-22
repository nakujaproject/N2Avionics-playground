
#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */
// #include "../include/functions.h"
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
  init_components();
}

void loop()
{
  struct SensorReadings readings;
  struct FilteredValues filtered_values;
  readings = get_readings();
  filtered_values = kalmanUpdate(readings.altitude, readings.az);
  state = checkState(filtered_values.displacement, filtered_values.velocity, counter, state);
  counter = counter + 1;
}
