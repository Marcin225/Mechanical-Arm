#ifndef PCA9685_H
#define PCA9685_H

#include "stm32l4xx_hal.h"

#define SERVO_MIN_TICK          102 // 0 degrees
#define SERVO_MAX_TICK          512 // 180 degrees

void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t frequency);
void PCA9685_SetPWM(uint8_t Channel, uint16_t OnTime, uint16_t OffTime);
void PCA9685_SetServoAngle(uint8_t Channel, uint8_t Angle);

#endif