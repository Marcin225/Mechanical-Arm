#include "PCA9685.h"

#define PCA9685_ADDRESS                 0x80  // 0x40 << 1
#define PCA9685_MODE1                   0x0
#define PCA9685_PRE_SCALE               0xFE
#define PCA9685_LED0_ON_L               0x6
#define PCA9685_MODE1_SLEEP             4
#define PCA9685_MODE1_AI                5
#define PCA9685_MODE1_RESTART           7


static I2C_HandleTypeDef *pca_i2c;

static void PCA9685_SetBit(uint8_t Register, uint8_t Bit, uint8_t Value)
{
    uint8_t readValue;
    HAL_I2C_Mem_Read(pca_i2c, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
    
    if (Value == 0) readValue &= ~(1 << Bit);
    else readValue |= (1 << Bit);
    
    HAL_I2C_Mem_Write(pca_i2c, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
    HAL_Delay(1);
}

static void PCA9685_SetPWMFrequency(uint16_t frequency)
{
    uint8_t prescale;
    if(frequency >= 1526) prescale = 0x03;
    else if(frequency <= 24) prescale = 0xFF;
    else prescale = 25000000 / (4096 * frequency);

    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 1);
    HAL_I2C_Mem_Write(pca_i2c, PCA9685_ADDRESS, PCA9685_PRE_SCALE, 1, &prescale, 1, 10);
    HAL_Delay(20);
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 0);
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_RESTART_BIT, 1);
}

void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t frequency)
{
    pca_i2c = hi2c; // Przypisanie wskaźnika
    PCA9685_SetPWMFrequency(frequency); 
    PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_AI_BIT, 1);
}

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

void PCA9685_SetServoAngle(uint8_t Channel, uint8_t Angle)
{
    // Zabezpieczenie przed błędem z zewnątrz
    if (Angle > 180) Angle = 180;

    // Przekształcona, znacznie szybsza matematyka stałoprzecinkowa 
    // Odpowiednik (Angle * (512 - 102) / 180) + 102
    // Skrócony ułamek (Angle * 410 / 180) to (Angle * 41 / 18)
    uint16_t Value = (Angle * 41) / 18 + SERVO_MIN_TICK;
    
    PCA9685_SetPWM(Channel, 0, Value);
}