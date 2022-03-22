#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SEA_LEVEL_PRESSURE_HPA  1024
#define CURRENT_ALTITUDE  1524
#define SEA_LEVEL_PRESSURE 102400

// Timing delays
#define SETUP_DELAY 1000

#define SDCARD_CS 23

#define SHORT_DELAY  10

#define BAUD_RATE 115200

#define UDP_PORT 4210

// value to detect lif off deviation
#define LIFT_OFF_DEVIATION 2 // ideal: 50 cm

struct LogData {
    float counter;
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
    int states;
    float longitude;
    float latitude;
};

struct SensorReadings {
    float altitude;
    float ax;
    float ay;
    float az;
};

struct FilteredValues {
    float displacement;
    float velocity;
    float acceleration;
};

#endif