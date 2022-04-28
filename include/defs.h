#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define SEA_LEVEL_PRESSURE 102400

// Timing delays
#define SETUP_DELAY 5000

// define sd card VSPI
#define SDCARD_CS_PIN 5
#define SD_MOSI_PIN 23
#define SD_MISO_PIN 19
#define SD_SCK_PIN 18

// define lora HSPI
#define LORA_CS_PIN 15
#define LORA_MOSI_PIN 13
#define LORA_MISO_PIN 12
#define LORA_SCK_PIN 14
#define RESET_LORA_PIN 25
#define IRQ_LORA_PIN 2

// define GPS UART pins
#define GPS_TX_PIN 17
#define GPS_RX_PIN 16

#define SHORT_DELAY 10

#define BAUD_RATE 115200

// Pin to start ignition
#define EJECTION_PIN 4

const char *telemetryLogFile = "telemetry.txt";

static float BASE_ALTITUDE;

static float MAX_ALTITUDE;

// Lora paramters
const long LORA_FREQ = 868E6; // frequechy 868 MHz
const int LORA_SF = 7;        // spread factor
const long LORA_BW = 125E3;   // bandwidth 125 kHz

#define LORA_SYNC_WORD 0xF3
// This struct is used to save all our datapoints.
// It includes rocket altitude, accelerations in the x, y and z directions
// Gryroscope values in the x, y and z direcion
// filtered altitude, velocity and acceleration
// GPS longitude, laltitude and altitude and state
struct LogData
{
    uint64_t timeStamp;
    float altitude;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float filtered_s;
    float filtered_v;
    float filtered_a;
    int state;
    float latitude;
    float longitude;
    float gpsAltitude;
};
// SensorReadings contains the measurement we are getting
// from the sensors bmp and mpu
struct SensorReadings
{
    float altitude;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
};
// GPSReadings contains the gps informations that is
// latitude, longitude, speed, number of satellites and altitude
struct GPSReadings
{
    float latitude;
    float longitude;
    float speed;
    int satellites;
    float altitude;
};

// FilteredValues contains filtered values from the kalman filter
struct FilteredValues
{
    float displacement;
    float velocity;
    float acceleration;
};
// SendValues contains the data points we will be sending over lora
struct SendValues
{
    uint64_t timeStamp;
    float altitude;
    uint16_t state;
};

#endif