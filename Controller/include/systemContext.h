#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

// central container grouping system variables to prevent code duplication 

#include <stdint.h>
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Wire.h>

#include "bleSender.h"
#include "mpu6050_driver.h"
#include "compFilter.h"

struct SystemContext {
    MPU6050 mpuSensor;

    int xRaw;
    int yValue;
    bool pressed;
    uint8_t mode_f;

    float roll_deg;
    float pitch_deg;

    int waist;
    int shoulder;
    int elbow;
    int wrist_roll;
    int wrist_pitch;
    int gripper;
};

#endif