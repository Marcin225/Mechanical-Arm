#include "PCA9685.h"

static I2C_HandleTypeDef *pca_i2c;

// modifies a single bit in a target register 

static void PCA9685_SetBit(uint8_t Register, uint8_t Bit, uint8_t Value)
{
    uint8_t readValue;
    HAL_I2C_Mem_Read(pca_i2c, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
    
    if (Value == 0) readValue &= ~(1 << Bit);
    else readValue |= (1 << Bit);
    
    HAL_I2C_Mem_Write(pca_i2c, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
    HAL_Delay(1);
}

// calculates and applies the internal oscillator prescaler for the desired PWM frequency
static void PCA9685_SetPWMFrequency(uint16_t frequency)
{
    uint8_t prescale;
    if(frequency >= 1526) prescale = 0x03;
    else if(frequency <= 24) prescale = 0xFF;
    else prescale = 25000000 / (4096 * frequency);

    // to update prescaler module must be put to sleep
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 1);
    HAL_I2C_Mem_Write(pca_i2c, PCA9685_ADDRESS, PCA9685_PRE_SCALE, 1, &prescale, 1, 10);
    HAL_Delay(20);
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 0);
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_RESTART_BIT, 1);
}

void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t frequency)
{
    pca_i2c = hi2c;
    PCA9685_SetPWMFrequency(frequency); 
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_AI_BIT, 1); // enable auto-increment 
}
// sends raw start and stop time ticks (0-4095) to a specific PWM channel
void PCA9685_SetPWM(uint8_t Channel, uint16_t OnTime, uint16_t OffTime)
{
    uint8_t registerAddress = PCA9685_LED0_ON_L + (4 * Channel);
    uint8_t pwm[4];
    
    pwm[0] = OnTime & 0xFF;
    pwm[1] = OnTime >> 8;
    pwm[2] = OffTime & 0xFF;
    pwm[3] = OffTime >> 8;
    
    HAL_I2C_Mem_Write(pca_i2c, PCA9685_ADDRESS, registerAddress, 1, pwm, 4, 10);
}

//maps a 0-180 degree angle into PWM ticks and moves the servo
void PCA9685_SetServoAngle(uint8_t Channel, uint8_t Angle)
{
    if (Angle > 180) Angle = 180;

    // converts 0-180 degrees into a 410-tick range (2 ms servo pulse)
    // 4095 -> 20 ms | 410 -> 2 ms
    uint16_t Value = (Angle * 41) / 18 + SERVO_MIN_TICK;
    
    PCA9685_SetPWM(Channel, 0, Value);
}