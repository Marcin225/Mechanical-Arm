#ifndef PCA9685_H
#define PCA9685_H

#include "stm32l4xx_hal.h"

#define SERVO_MIN_TICK          102 // 0 degrees
#define SERVO_MAX_TICK          512 // 180 degrees

#define PCA9685_ADDRESS                 0x80  // 0x40 << 1
#define PCA9685_MODE1                   0x0
#define PCA9685_PRE_SCALE               0xFE
#define PCA9685_LED0_ON_L               0x6
#define PCA9685_MODE1_SLEEP_BIT         4
#define PCA9685_MODE1_AI_BIT            5
#define PCA9685_MODE1_RESTART_BIT       7

void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t frequency);
void PCA9685_SetPWM(uint8_t Channel, uint16_t OnTime, uint16_t OffTime);
void PCA9685_SetServoAngle(uint8_t Channel, uint8_t Angle);

#endif