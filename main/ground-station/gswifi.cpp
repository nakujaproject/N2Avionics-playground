#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char *ssid = "appendix";
const char *key = "123456789";

HTTPClient http;
WiFiUDP udp;

// Your IP address or domain name with URL path
const char *serverURL = "http://192.168.4.1/data";

// Variable to store the HTTP request
String header;
String data;

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 500;

String getData(const char *server_name)
{
    WiFiClient client;
    HTTPClient http;

    // IP address with path
    http.begin(client, server_name);

    // send HTTP POST request from server to client
    int http_response_code = http.GET();

    String payload = "--";

    if (http_response_code == 200)
    {
        Serial.print("HTTP response code:");
        Serial.print(http_response_code);
        payload = http.getString();
    }
    else
    {
        // print error code. logging file can be done here. Send this code to python and create a logger file
        Serial.print("Error code: ");
        Serial.println(http_response_code);
    }

    // free hardware resources being used
    http.end();

    // return the fetched data
    return payload;
}

void setup()
{
    Serial.begin(115200);

    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.begin(ssid, key);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting...");
        delay(200);
    }

    Serial.print("Connected to Wi-Fi...");
}

void loop()
{

    currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {

            // print the received signal strength if connected
            long rssi = WiFi.RSSI();
            Serial.print("RSSI:");
            Serial.print(rssi);
            Serial.println();

            data = getData(serverURL);
            // Serial.println(data);

            DynamicJsonDocument doc(384);
            DeserializationError error = deserializeJson(doc, data);

            // Test if parsing succeeds.
            if (!error)
            {
                int counter = doc["counter"];
                float altitude = doc["altitude"];
                float ax = doc["ax"];
                float ay = doc["ay"];
                float az = doc["az"];
                float gx = doc["gx"];
                float gy = doc["gy"];
                float gz = doc["gz"];
                float filtered_s = doc["filtered_s"];
                float filtered_v = doc["filtered_v"];
                float filtered_a = doc["filtered_a"];
                int state = doc["state"];
                float longitude = doc["longitude"];
                float latitude = doc["latitude"];

                Serial.printf("Counter : %d\n", counter);
                Serial.printf("Altitude : %f\n", altitude);
                Serial.printf("Ax : %f\n", ax);
                Serial.printf("Ay : %f\n", ay);
                Serial.printf("Az : %f\n", az);
                Serial.printf("Gx : %f\n", gx);
                Serial.printf("Gy : %f\n", gy);
                Serial.printf("Gz : %f\n", gz);
                Serial.printf("Filtered s : %f\n", filtered_s);
                Serial.printf("Filtered v %f\n", filtered_v);
                Serial.printf("Filtered a %f\n", filtered_a);
                Serial.printf("State : %d\n", state);
                Serial.printf("Longitude : %f\n", longitude);
                Serial.printf("Latitude : %f\n", latitude);
            }
            else
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            previousMillis = currentMillis;
        }
    }
}
