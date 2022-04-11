// #include <Arduino.h>
// #include <LoRa.h>

// #include <ArduinoJson.h>

// const long freq = 868E6;
// const int SF = 9;
// const long bw = 125E3;

// void setUpLoraOnBoard()
// {
//     LoRa.setPins(5, 25, 2); // set CS, reset, IRQ pin
//     Serial.println("Setting up LoRa Receiver");

//     if (!LoRa.begin(freq))
//     {
//         Serial.print("Failed to begin");
//     }

//     Serial.println();
//     Serial.println("Successfully set up LoRa");

//     LoRa.setSpreadingFactor(SF);
//     // LoRa.setSignalBandwidth(bw);
//     LoRa.setSyncWord(0xF3);
//     Serial.print("Frequency ");
//     Serial.print(freq);
//     Serial.print(" Bandwidth ");
//     Serial.print(bw);
//     Serial.print(" SF ");
//     Serial.println(SF);
// }

// void setup()
// {
//     Serial.begin(115200);
//     setUpLoraOnBoard();
// }
// void loop()
// {
//     // try to parse packet
//     int packetSize = LoRa.parsePacket();
//     if (packetSize)
//     {
//         // received a packet
//         String message = "";
//         while (LoRa.available())
//         {
//             message = LoRa.readString();
//         }
//         // Serial.print("RSSI : ");
//         // Serial.println(LoRa.packetRssi());

//         // DynamicJsonDocument doc(384);
//         // DeserializationError error = deserializeJson(doc, message);

//         // // Test if parsing succeeds.
//         // if (!error)
//         // {
//         //     int counter = doc["Counter"];
//         //     float altitude = doc["Altitude"];
//         //     float ax = doc["ax"];
//         //     float ay = doc["ay"];
//         //     float az = doc["az"];
//         //     float gx = doc["gx"];
//         //     float gy = doc["gy"];
//         //     float gz = doc["gz"];
//         //     float filtered_s = doc["s"];
//         //     float filtered_v = doc["v"];
//         //     float filtered_a = doc["a"];
//         //     int state = doc["Current State"];
//         //     float longitude = doc["Longitude"];
//         //     float latitude = doc["Latitude"];

//         //     Serial.printf("Counter : %d\n", counter);
//         //     Serial.printf("Altitude : %f\n", altitude);
//         //     Serial.printf("Ax : %f\n", ax);
//         //     Serial.printf("Ay : %f\n", ay);
//         //     Serial.printf("Az : %f\n", az);
//         //     Serial.printf("Gx : %f\n", gx);
//         //     Serial.printf("Gy : %f\n", gy);
//         //     Serial.printf("Gz : %f\n", gz);
//         //     Serial.printf("Filtered s : %f\n", filtered_s);
//         //     Serial.printf("Filtered v %f\n", filtered_v);
//         //     Serial.printf("Filtered a %f\n", filtered_a);
//         //     Serial.printf("State : %d\n", state);
//         //     Serial.printf("Longitude : %f\n", longitude);
//         //     Serial.printf("Latitude : %f\n", latitude);
//         //     Serial.println();
//         //     Serial.println();
//         // }
//         // else
//         // {
//         //     Serial.print(F("deserializeJson() failed: "));
//         //     Serial.println(error.f_str());
//         //     return;
//         // }

//         Serial.println(message);
//     }
// }