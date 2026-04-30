#include "Arduino.h"
#include "Wire.h"
#include "mpu6050_driver.h"

MPU6050::MPU6050(void) {
    _i2caddr = MPU6050_I2C_ADDRESS;
}

void MPU6050::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_i2caddr);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

int MPU6050::readRegister(uint8_t reg) {
    Wire.beginTransmission(_i2caddr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        return -1;
    }

    int n = Wire.requestFrom(_i2caddr, (uint8_t) 1);
    if (n != 1) {
        return -1;
    }

    uint8_t value = 0;

    if (Wire.available()) {
        return value = Wire.read();
    }

    return -1;
}

bool MPU6050::begin() {
    if (readRegister(MPU6050_WHO_AM_I) != MPU6050_DEVICE_ID)
        return false;

    writeRegister(MPU6050_PWR_MGMT_1, 0x80); // 0x80 - Reset

    bool resetSucces = false;
    delay(10);

    unsigned long startTime = millis();
    while (millis() - startTime < 100)
    {
        if ((readRegister(MPU6050_PWR_MGMT_1) & 0x80) == 0) {
            resetSucces = true;
            break;
        }
        
        delay(5);
    }

    if (!resetSucces) {
        return false;
    }

    writeRegister(MPU6050_PWR_MGMT_1, 0x01); // Wakes up the MPU6050 
            //and sets the clock source to the X-axis gyroscope PLL instead of the internal 8 MHz oscillator
    delay(10);
    setup();

    return true;
}

void MPU6050::setup() {
    writeRegister(MPU6050_SMPLRT_DIV, 0x09); // 0x09 - Sample Rate (100 hz) = Gyroscope Output Rate / (1 + SMPLRT_DIV)

    writeRegister(MPU6050_CONFIG, 0x03); // 0x03 - 44 Hz Bandwidth

    writeRegister(MPU6050_GYRO_CONFIG, 0x08); // 0x08 - 500 degrees / s

    writeRegister(MPU6050_ACCEL_CONFIG, 0x00); // 0x00 - full scale range 2g
}

bool MPU6050::readSample(MpuSample& sample) {
    Wire.beginTransmission(_i2caddr);
    Wire.write(MPU6050_ACCEL_XOUT_H);
    
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    Wire.requestFrom(_i2caddr, (uint8_t) 14);

    if (Wire.available() < 14) {
        return false;
    }

    uint8_t buffer[14];
    for (int i = 0; i < 14; i++) {
        buffer[i] = Wire.read();
    }

    sample.accX = (int16_t) ((buffer[0] << 8) | buffer[1]);
    sample.accY = (int16_t) ((buffer[2] << 8) | buffer[3]);
    sample.accZ = (int16_t) ((buffer[4] << 8) | buffer[5]);

    // temperature bytes skiping

    sample.gyroX = (int16_t) ((buffer[8] << 8) | buffer[9]);
    sample.gyroY = (int16_t) ((buffer[10] << 8) | buffer[11]);
    sample.gyroZ = (int16_t) ((buffer[12] << 8) | buffer[13]);

    return true;
}