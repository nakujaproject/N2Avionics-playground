
#include <Arduino.h>
/*
 * Check brownout issues to prevent ESP32 from re-booting unexpectedly
 */
#include "../include/functions.h"
#include "../include/kalmanfilter.h"
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/transmitt.h"
#include "../include/readsensors.h"

// send data to ground station

void setup()
{
  init_components();
  Serial.begin(BAUD_RATE);
  createAccessPoint();
  setupServer();
}

void loop()
{
  server.handleClient();
}
