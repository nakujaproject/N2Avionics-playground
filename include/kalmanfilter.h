#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <BasicLinearAlgebra.h>
#include "defs.h"

using namespace BLA;

float q = 0.0001;

float T = 0.1;

// The system dynamics
BLA::Matrix<3, 3> A = {1.0, 0.1, 0.005,
                       0, 1.0, 0.1,
                       0, 0, 1.0};

// Relationship between measurement and states
BLA::Matrix<2, 3> H = {1.0, 0, 0,
                       0, 0, 1.0};

// Initial posteriori estimate error covariance
BLA::Matrix<3, 3> P = {1, 0, 0,
                       0, 1, 0,
                       0, 0, 1};

// Measurement error covariance
BLA::Matrix<2, 2> R = {0.25, 0,
                       0, 0.75};

// Process noise covariance
BLA::Matrix<3, 3> Q = {q, q, q,
                       q, q, q,
                       q, q, q};

// Identity Matrix
BLA::Matrix<3, 3> I = {1, 0, 0,
                       0, 1, 0,
                       0, 0, 1};

BLA::Matrix<3, 1> x_hat = {1500.0,
                           0.0,
                           0.0};

BLA::Matrix<2, 1> Y = {0.0,
                       0.0};

struct FilteredValues kalmanUpdate(float altitude, float az)
{
    struct FilteredValues return_val;
    // Measurement matrix

    BLA::Matrix<2, 1> Z = {altitude,
                           az};
    // Predicted state estimate
    BLA::Matrix<3, 1> x_hat_minus = A * x_hat;
    // Predicted estimate covariance
    BLA::Matrix<3, 3> P_minus = A * P * (~A) + Q;
    // Kalman gain
    BLA::Matrix<3, 2> K = P_minus * (~H) * ((H * P_minus * (~H) + R));
    // Measurement residual
    Y = Z - (H * x_hat_minus);
    // Updated state estimate
    x_hat = x_hat_minus + K * Y;
    // Updated estimate covariance
    P = (I - K * H) * P_minus;
    Y = Z - (H * x_hat_minus);

    return_val.displacement = x_hat(0);
    return_val.velocity = x_hat(1);
    return_val.acceleration = x_hat(2);

    return return_val;
}
struct LogData readData()
{
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

    return ld;
}

#endif