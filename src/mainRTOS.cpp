// #include <Arduino.h>
// /*
//  * Check brownout issues to prevent ESP32 from re-booting unexpectedly
//  */
// #include "../include/functions.h"
// #include "../include/functions.h"
// #include "../include/kalmanfilter.h"
// #include "../include/checkState.h"
// #include "../include/logdata.h"
// #include "../include/readsensors.h"
// #include "../include/transmitlora.h"
// #include "../include/defs.h"
// #include <SPI.h>

// TaskHandle_t LoRaTaskHandle;
// TaskHandle_t ReadingsTaskHandle;
// TaskHandle_t LogDataTaskHandle;
// TaskHandle_t FlightControlTaskHandle;

// // uninitalised pointers to SPI objects
// SPIClass *hspi = NULL;

// void LoRaTask(void *params)
// {
//     for (;;)
//     {
//     }
// }

// void ReadingsTask(void *params)
// {
//     for (;;)
//     {
//     }
// }
// void LogDataTask(void *params)
// {
//     for (;;)
//     {
//         startWriting();
//     }
// }
// void FlightControlTask(void *params)
// {
//     for (;;)
//     {
//     }
// }

// void setup()
// {

//     Serial.begin(115200);

//     // set up slave select pins as outputs as the Arduino API
//     pinMode(LORA_CS_PIN, OUTPUT);   // HSPI SS for use by LORA
//     pinMode(SDCARD_CS_PIN, OUTPUT); // VSPI SS for use by SDCARD

//     // set up parachute pin
//     pinMode(EJECTION_PIN, OUTPUT);

//     hspi = new SPIClass(HSPI);
//     hspi->begin();

//     SPIClass &spi = *hspi;
//     setUpLoraOnBoard(spi);

//     init_components();

//     // initialize core tasks
//     xTaskCreatePinnedToCore(LoRaTask, "LoRaTask", 10000, NULL, 1, &LoRaTaskHandle, 0);
//     xTaskCreatePinnedToCore(ReadingsTask, "ReadingsTask", 10000, NULL, 1, &ReadingsTaskHandle, 1);
//     xTaskCreatePinnedToCore(LogDataTask, "LogDataTask", 10000, NULL, 1, &LogDataTaskHandle, 1);
//     xTaskCreatePinnedToCore(FlightControlTask, "FlightControlTask", 10000, NULL, 1, &FlightControlTaskHandle, 1);
// }

// void loop() {}

